// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/buffer.hh"

#include <mio/mmap.hpp>

#include <fstream>

namespace phoenix {
	namespace detail {
		/// \brief A buffer backing which saves data on the heap.
		class HeapBacking : public BufferBacking {
		public:
			/// \brief Creates a new, empty heap buffer backing with the given size.
			/// \param size The number of bytes in the backing.
			explicit HeapBacking(std::uint64_t size) : _m_readonly(false) {
				_m_data.resize(size);
			}

			/// \brief Creates a new heap buffer backing from the given vector.
			/// \param buf The data to insert into the backing.
			/// \param readonly Set to `false` to allow writing to the backing.
			HeapBacking(std::vector<std::byte>&& buf, bool readonly) : _m_data(std::move(buf)), _m_readonly(readonly) {}

			[[nodiscard]] bool direct() const noexcept override {
				return false;
			}

			[[nodiscard]] bool readonly() const noexcept override {
				return _m_readonly;
			}

			[[nodiscard]] uint64_t size() const noexcept override {
				return _m_data.size();
			}

			[[nodiscard]] const std::byte* array() const override {
				return _m_data.data();
			}

			void read(std::byte* buf, std::uint64_t size, std::uint64_t offset) const override {
				std::copy_n(_m_data.cbegin() + static_cast<long>(offset), size, buf);
			}

			void write([[maybe_unused]] const std::byte* buf,
			           [[maybe_unused]] std::uint64_t size,
			           [[maybe_unused]] std::uint64_t offset) override {
				if (this->readonly()) {
					throw BufferReadonlyError {};
				}

				if (offset + size > this->size()) {
					throw BufferOverflowError {offset, size, "in backing"};
				}

				std::copy_n(buf, size, const_cast<std::byte*>(_m_data.data()) + static_cast<long>(offset));
			}

		private:
			std::vector<std::byte> _m_data;
			bool _m_readonly;
		};

		/// \brief A buffer backing for memory-mapped files.
		template <mio::access_mode mode>
		class MmapBacking : public BufferBacking {
		public:
			/// \brief Creates a new memory-mapped buffer backing by mapping the file at the given path into memory
			/// \param file The path of the file to map
			explicit MmapBacking(const std::filesystem::path& file) : _m_data(file.c_str()) {}

			[[nodiscard]] bool direct() const noexcept override {
				return true;
			}

			[[nodiscard]] bool readonly() const noexcept override {
				return mode == mio::access_mode::read;
			}

			[[nodiscard]] uint64_t size() const noexcept override {
				return _m_data.size();
			}

			[[nodiscard]] const std::byte* array() const noexcept override {
				return _m_data.data();
			}

			void read(std::byte* buf, std::uint64_t size, std::uint64_t offset) const override {
				std::copy_n(_m_data.cbegin() + static_cast<long>(offset), size, buf);
			}

			void write([[maybe_unused]] const std::byte* buf,
			           [[maybe_unused]] std::uint64_t size,
			           [[maybe_unused]] std::uint64_t offset) override {
				if (this->readonly()) {
					throw BufferReadonlyError {};
				}

				if (offset + size > this->size()) {
					throw BufferOverflowError {offset, size, "in backing"};
				}

				std::copy_n(buf, size, const_cast<std::byte*>(_m_data.data()) + static_cast<long>(offset));
			}

		private:
			mio::basic_mmap<mode, std::byte> _m_data;
		};
	} // namespace detail

	BufferUnderflowError::BufferUnderflowError(std::uint64_t off, std::uint64_t rsize)
	    : BufferError("buffer underflow at byte " + std::to_string(off) + " while reading " + std::to_string(rsize) +
	                  " additional bytes"),
	      byte(off), size(rsize), context(std::nullopt) {}

	BufferUnderflowError::BufferUnderflowError(std::uint64_t off, std::uint64_t rsize, std::string&& ctx)
	    : BufferError("buffer underflow at byte " + std::to_string(off) + " while reading " + std::to_string(rsize) +
	                  " additional bytes [context: " + ctx + "]"),
	      byte(off), size(rsize), context(std::move(ctx)) {}

	BufferUnderflowError::BufferUnderflowError(std::uint64_t off, std::string&& ctx)
	    : BufferError("buffer underflow at byte " + std::to_string(off) + " [context: " + ctx + "]"), byte(off),
	      size(0), context(std::move(ctx)) {}

	BufferOverflowError::BufferOverflowError(std::uint64_t off, std::uint64_t wsize)
	    : BufferError("buffer overflow at byte " + std::to_string(off) + " while writing " + std::to_string(wsize) +
	                  " additional bytes"),
	      byte(off), size(wsize), context(std::nullopt) {}

	BufferOverflowError::BufferOverflowError(std::uint64_t off, std::uint64_t wsize, std::string&& ctx)
	    : BufferError("buffer overflow at byte " + std::to_string(off) + " while writing " + std::to_string(wsize) +
	                  " additional bytes [context: " + ctx + "]"),
	      byte(off), size(wsize), context(std::move(ctx)) {}

	std::unique_ptr<Buffer> Buffer::_m_empty {};

	Buffer::Buffer(std::shared_ptr<BufferBacking> backing)
	    : _m_backing(std::move(backing)), _m_backing_begin(0), _m_backing_end(_m_backing->size()),
	      _m_capacity(_m_backing->size()), _m_position(0) {}

	Buffer::Buffer(std::shared_ptr<BufferBacking> backing, std::uint64_t begin, std::uint64_t end)
	    : _m_backing(std::move(backing)), _m_backing_begin(begin), _m_backing_end(end), _m_capacity(end - begin),
	      _m_position(0) {}

	Buffer::Buffer(std::shared_ptr<BufferBacking> backing,
	               std::uint64_t begin,
	               std::uint64_t end,
	               std::uint64_t capacity,
	               std::uint64_t position,
	               std::optional<std::uint64_t> mark)
	    : _m_backing(std::move(backing)), _m_backing_begin(begin), _m_backing_end(end), _m_capacity(capacity),
	      _m_position(position), _m_mark(mark) {}

	Buffer Buffer::allocate(std::uint64_t size) {
		return Buffer {std::make_shared<detail::HeapBacking>(size)};
	}

	Buffer Buffer::of(std::vector<std::byte>&& buf, bool readonly) {
		return Buffer {std::make_shared<detail::HeapBacking>(std::forward<std::vector<std::byte>>(buf), readonly)};
	}

	Buffer Buffer::mmap(const std::filesystem::path& path, bool readonly) {
		auto file_size = std::filesystem::file_size(path);
		if (file_size == 0)
			return Buffer::empty();

		if (readonly) {
			return Buffer {std::make_shared<detail::MmapBacking<mio::access_mode::read>>(path)};
		}

		return Buffer {std::make_shared<detail::MmapBacking<mio::access_mode::write>>(path)};
	}

	Buffer Buffer::read(const std::filesystem::path& path, bool readonly) {
		std::ifstream in {path, std::ios::binary | std::ios::ate};
		std::vector<std::byte> data {static_cast<size_t>(in.tellg())};

		in.seekg(0);
		in.read((char*) data.data(), data.size());

		return Buffer::of(std::move(data), readonly);
	}

	Buffer Buffer::empty() {
		if (Buffer::_m_empty == nullptr) {
			// If we don't have an empty buffer yet, create one.
			Buffer::_m_empty = std::make_unique<Buffer>(Buffer::allocate(0));
		}

		return Buffer::_m_empty->duplicate();
	}

	void Buffer::clear() noexcept {
		_m_position = 0;
		_m_backing_end = _m_backing_begin + _m_capacity;
		_m_mark.reset();
	}

	Buffer Buffer::duplicate() const noexcept {
		return Buffer {_m_backing, _m_backing_begin, _m_backing_end, _m_capacity, _m_position, _m_mark};
	}

	void Buffer::flip() noexcept {
		_m_backing_end = _m_backing_begin + _m_position;
		_m_position = 0;
		_m_mark.reset();
	}

	void Buffer::limit(std::uint64_t limit) {
		if (limit > this->capacity()) {
			throw BufferUnderflowError {limit, "setting limit"};
		}

		_m_position = std::min(limit, _m_position);
		_m_backing_end = _m_backing_begin + limit;

		if (_m_mark && *_m_mark > limit) {
			_m_mark.reset();
		}
	}

	void Buffer::position(std::uint64_t pos) {
		if (pos > this->limit()) {
			throw BufferUnderflowError {pos, "setting position"};
		}

		if (_m_mark && *_m_mark > pos) {
			_m_mark.reset();
		}

		_m_position = pos;
	}

	Buffer Buffer::slice() const noexcept {
		return Buffer {_m_backing, _m_backing_begin + _m_position, _m_backing_end};
	}

	Buffer Buffer::slice(std::uint64_t index, std::uint64_t size) const {
		if (index + size > this->limit()) {
			throw BufferUnderflowError {index, size, "slicing"};
		}

		return Buffer {_m_backing, _m_backing_begin + index, _m_backing_begin + index + size};
	}

	void Buffer::get(std::byte* buf, std::uint64_t size) {
		if (this->remaining() < size) {
			throw BufferUnderflowError {this->position(), size, "relative bulk get"};
		}

		_m_backing->read(buf, size, _m_backing_begin + _m_position);
		_m_position += size;
	}

	std::string Buffer::get_string(std::uint64_t size) {
		if (this->remaining() < size) {
			throw BufferOverflowError {position(), size, "relative string get"};
		}

		std::string tmp {};
		tmp.resize(size);
		this->get((std::byte*) tmp.data(), size);
		return tmp;
	}

	std::string Buffer::get_line(bool skip_whitespace) {
		if (skip_whitespace) {
			return this->get_line_and_ignore(" \f\n\r\t\v");
		}

		return this->get_line_and_ignore("");
	}

	std::string Buffer::get_line_and_ignore(std::string_view whitespace) {
		std::string tmp {};

		// Fix for #70, avoid attempting to read bytes from a
		// buffer which has reached its limit.
		if (this->remaining() == 0) {
			return "";
		}

		char c = this->get_char();
		while (c != '\n' && c != '\r' && c != '\0' && this->remaining() > 0) {
			tmp.push_back(c);
			c = this->get_char();
		}

		if (!whitespace.empty() && remaining() > 0) {
			c = this->get_char();
			while (whitespace.find(c) != std::string_view::npos && remaining() > 0) {
				c = this->get_char();
			}

			if (remaining() != 0) {
				this->position(this->position() - 1);
			}
		}

		return tmp;
	}

	std::string Buffer::get_line_escaped(bool skip_whitespace) {
		auto tmp = this->get_line(skip_whitespace);

		for (auto i = 0u; i < tmp.size(); ++i) {
			if (tmp[i] == '\\') {
				switch (tmp[i + 1]) {
				case 'n':
					tmp[i] = '\n';
					tmp.erase(i + 1, 1);
					break;
				case 't':
					tmp[i] = '\t';
					tmp.erase(i + 1, 1);
					break;
				}
			}
		}

		return tmp;
	}

	void Buffer::put(const std::byte* buf, std::uint64_t size) {
		if (this->remaining() < size) {
			throw BufferOverflowError {this->position(), size, "relative bulk put"};
		}

		_m_backing->write(buf, size, _m_backing_begin + _m_position);
		_m_position += size;
	}

	void Buffer::put_string(std::string_view str) {
		this->put((const std::byte*) str.data(), str.size());
	}

	void Buffer::put_line(std::string_view str) {
		put_string(str);
		put_char('\n');
	}

	bool operator==(const Buffer& self, const Buffer& other) {
		return &other == &self ||
		    (other._m_backing == self._m_backing && other._m_backing_begin == self._m_backing_begin &&
		     other._m_backing_end == self._m_backing_end && other._m_capacity == self._m_capacity &&
		     other._m_position == self._m_position);
	}

	std::uint8_t Buffer::get() {
		return _get_t<std::uint8_t>();
	}

	char Buffer::get_char() {
		return _get_t<char>();
	}

	std::int16_t Buffer::get_short() {
		return _get_t<std::int16_t>();
	}

	std::uint16_t Buffer::get_ushort() {
		return _get_t<std::uint16_t>();
	}

	std::int32_t Buffer::get_int() {
		return _get_t<std::int32_t>();
	}

	std::uint32_t Buffer::get_uint() {
		return _get_t<std::uint32_t>();
	}

	std::int64_t Buffer::get_long() {
		return _get_t<std::int64_t>();
	}

	std::uint64_t Buffer::get_ulong() {
		return _get_t<std::uint64_t>();
	}

	float Buffer::get_float() {
		return _get_t<float>();
	}

	double Buffer::get_double() {
		return _get_t<double>();
	}

	glm::vec2 Buffer::get_vec2() {
		float content[2];
		this->get((std::byte*) content, sizeof(content));
		return {content[0], content[1]};
	}

	glm::vec3 Buffer::get_vec3() {
		float content[3];
		this->get((std::byte*) content, sizeof(content));
		return glm::vec3 {content[0], content[1], content[2]};
	}

	glm::mat3x3 Buffer::get_mat3x3() {
		float content[3 * 3];
		this->get((std::byte*) content, sizeof(content));
		return glm::transpose(glm::mat3x3 {
		    content[0],
		    content[1],
		    content[2],
		    content[3],
		    content[4],
		    content[5],
		    content[6],
		    content[7],
		    content[8],
		});
	}

	glm::mat4x4 Buffer::get_mat4x4() {
		float content[4 * 4];
		this->get((std::byte*) content, sizeof(content));
		return glm::transpose(glm::mat4x4 {
		    content[0],
		    content[1],
		    content[2],
		    content[3],
		    content[4],
		    content[5],
		    content[6],
		    content[7],
		    content[8],
		    content[9],
		    content[10],
		    content[11],
		    content[12],
		    content[13],
		    content[14],
		    content[15],
		});
	}

	glm::vec4 Buffer::get_vec4() {
		float content[4];
		this->get((std::byte*) content, sizeof(content));
		return glm::vec4 {content[0], content[1], content[2], content[3]};
	}

	void Buffer::put(const std::uint8_t* buf, std::uint64_t size) {
		this->put((const std::byte*) buf, size);
	}

	void Buffer::put(std::uint8_t value) {
		_put_t(value);
	}

	void Buffer::put_char(char value) {
		_put_t(value);
	}

	void Buffer::put_short(std::int16_t value) {
		_put_t(value);
	}

	void Buffer::put_ushort(std::uint16_t value) {
		_put_t(value);
	}

	void Buffer::put_int(std::int32_t value) {
		_put_t(value);
	}

	void Buffer::put_uint(std::uint32_t value) {
		_put_t(value);
	}

	void Buffer::put_long(std::int64_t value) {
		_put_t(value);
	}

	void Buffer::put_ulong(std::uint64_t value) {
		_put_t(value);
	}

	void Buffer::put_float(float value) {
		_put_t(value);
	}

	void Buffer::put_double(double value) {
		_put_t(value);
	}

	template <typename T, typename>
	PHOENIX_INTERNAL void Buffer::_put_t(T value) {
		if (this->remaining() < sizeof(T)) {
			throw BufferOverflowError {this->position(), sizeof(T)};
		}

		_m_backing->write((std::byte*) &value, sizeof(T), _m_backing_begin + _m_position);
		_m_position += sizeof(T);
	}

	template <typename T, typename>
	PHOENIX_INTERNAL T Buffer::_get_t(std::uint64_t pos) const {
		if (pos + sizeof(T) > limit()) {
			throw BufferUnderflowError {pos, sizeof(T)};
		}

		T tmp;
		_m_backing->read((std::byte*) &tmp, sizeof(T), _m_backing_begin + pos);
		return tmp;
	}

	template <typename T, typename>
	PHOENIX_INTERNAL T Buffer::_get_t() {
		auto tmp = this->_get_t<T>(this->position());
		_m_position += sizeof(T);
		return tmp;
	}
} // namespace phoenix
