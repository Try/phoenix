// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/ModelScript.hh"
#include "phoenix/Phoenix.hh"

#include "ModelScriptDsl"

#include <iostream>
#include <unordered_map>

namespace phoenix {
	enum class ModelScriptBinaryChunkType : uint16_t {
		// model_mesh = 0xD000,
		ROOT = 0xF000,
		END = 0xFFFF,
		SOURCE = 0xF100,
		// model = 0xF200,
		// model_end = 0xF2FF,
		MESH_AND_TREE = 0xF300,
		REGISTER_MESH = 0xF400,
		// animation_enum = 0xF500,
		// animation_enum_end = 0xF5FF,
		// CHUNK_ANI_MAX_FPS           = 0xF510,
		ANIMATION = 0xF520,
		ANIMATION_ALIAS = 0xF530,
		ANIMATION_BLEND = 0xF540,
		// CHUNK_ANI_SYNC              = 0xF550,
		// CHUNK_ANI_BATCH             = 0xF560,
		ANIMATION_COMBINE = 0xF570,
		ANIMATION_DISABLE = 0xF580,
		MODEL_TAG = 0xF590,
		ANIMATION_EVENTS = 0xF5A0,
		// animation_events_end = 0xF5AF,
		EVENT_SFX = 0xF5A1,
		EVENT_SFX_GROUND = 0xF5A2,
		EVENT_TAG = 0xF5A3,
		EVENT_PFX = 0xF5A4,
		EVENT_PFX_STOP = 0xF5A5,
		// CHUNK_EVENT_PFX_GRND        = 0xF5A6,
		// CHUNK_EVENT_SET_MESH        = 0xF5A7,
		// CHUNK_EVENT_SWAP_MESH       = 0xF5A8,
		EVENT_MM_ANI = 0xF5A9,
		EVENT_CAMERA_TREMOR = 0xF5AA,
		UNKNOWN,
	};

	static const std::unordered_map<std::string, MdsEventType> event_types {
	    {"DEF_CREATE_ITEM", MdsEventType::ITEM_CREATE},
	    {"DEF_INSERT_ITEM", MdsEventType::ITEM_INSERT},
	    {"DEF_REMOVE_ITEM", MdsEventType::ITEM_REMOVE},
	    {"DEF_DESTROY_ITEM", MdsEventType::ITEM_DESTROY},
	    {"DEF_PLACE_ITEM", MdsEventType::ITEM_PLACE},
	    {"DEF_EXCHANGE_ITEM", MdsEventType::ITEM_EXCHANGE},
	    {"DEF_FIGHTMODE", MdsEventType::SET_FIGHT_MODE},
	    {"DEF_PLACE_MUNITION", MdsEventType::MUNITION_PLACE},
	    {"DEF_REMOVE_MUNITION", MdsEventType::MUNITION_REMOVE},
	    {"DEF_DRAWSOUND", MdsEventType::SOUND_DRAW},
	    {"DEF_UNDRAWSOUND", MdsEventType::SOUND_UNDRAW},
	    {"DEF_SWAPMESH", MdsEventType::MESH_SWAP},
	    {"DEF_DRAWTORCH", MdsEventType::TORCH_DRAW},
	    {"DEF_INV_TORCH", MdsEventType::TORCH_INVENTORY},
	    {"DEF_DROP_TORCH", MdsEventType::TORCH_DROP},
	    {"DEF_HIT_LIMB", MdsEventType::HIT_LIMB},
	    {"HIT_LIMB", MdsEventType::HIT_LIMB},
	    {"DEF_HIT_DIR", MdsEventType::HIT_DIRECTION},
	    {"DEF_DIR", MdsEventType::HIT_DIRECTION}, // TODO: Validate this!
	    {"DEF_DAM_MULTIPLY", MdsEventType::DAMAGE_MULTIPLIER},
	    {"DEF_PAR_FRAME", MdsEventType::PARRY_FRAME},
	    {"DEF_OPT_FRAME", MdsEventType::OPTIMAL_FRAME},
	    {"DEF_HIT_END", MdsEventType::HIT_END},
	    {"DEF_WINDOW", MdsEventType::COMBO_WINDOW},
	};

	namespace mds {
		MdsEventTag make_event_tag(int32_t frame,
		                           std::string&& type,
		                           std::optional<std::string>&& a,
		                           std::optional<std::string>&& b,
		                           bool attached) {
			MdsEventTag evt {};
			evt.frame = frame;

			auto tp = event_types.find(type);
			if (tp == event_types.end()) {
				evt.type = MdsEventType::UNKNOWN;
				PX_LOGW("ModelScript: unexpected value for MdsEventType: \"", type, "\"");
			} else {
				evt.type = tp->second;
			}

			switch (evt.type) {
			case MdsEventType::ITEM_CREATE:
			case MdsEventType::ITEM_EXCHANGE:
				evt.slot = a.value_or("");
				evt.item = b.value_or("");
				break;
			case MdsEventType::ITEM_INSERT:
			case MdsEventType::MUNITION_PLACE:
				evt.slot = a.value_or("");
				break;
			case MdsEventType::SET_FIGHT_MODE: {
				auto mode = a.value_or("");

				if (mode == "FIST") {
					evt.fight_mode = MdsFightMode::FIST;
				} else if (mode == "1H" || mode == "1h") {
					evt.fight_mode = MdsFightMode::SINGLE_HANDED;
				} else if (mode == "2H" || mode == "2h") {
					evt.fight_mode = MdsFightMode::DUAL_HANDED;
				} else if (mode == "BOW") {
					evt.fight_mode = MdsFightMode::BOW;
				} else if (mode == "CBOW") {
					evt.fight_mode = MdsFightMode::CROSSBOW;
				} else if (mode == "MAG") {
					evt.fight_mode = MdsFightMode::MAGIC;
				} else {
					evt.fight_mode = MdsFightMode::NONE;
				}
				break;
			}
			case MdsEventType::MESH_SWAP:
				evt.slot = a.value_or("");
				evt.slot2 = b.value_or("");
				break;
			case MdsEventType::HIT_LIMB:
				(void) a.value_or("");
				break;
			case MdsEventType::DAMAGE_MULTIPLIER:
			case MdsEventType::PARRY_FRAME:
			case MdsEventType::OPTIMAL_FRAME:
			case MdsEventType::HIT_END:
			case MdsEventType::COMBO_WINDOW: {
				auto frames = a.value_or("");
				std::istringstream stream {frames};

				int32_t fr = 0;
				while (!stream.eof()) {
					stream >> fr;
					evt.frames.push_back(fr);
				}
				break;
			}
			default:
				break;
			}

			evt.attached = attached;
			return evt;
		}

		AnimationFlags animation_flags_from_string(std::string_view str) {
			AnimationFlags flags = AnimationFlags::NONE;

			for (char c : str) {
				switch (c) {
				case 'M':
					flags |= AnimationFlags::MOVE;
					break;
				case 'R':
					flags |= AnimationFlags::ROTATE;
					break;
				case 'E':
					flags |= AnimationFlags::QUEUE;
					break;
				case 'F':
					flags |= AnimationFlags::FLY;
					break;
				case 'I':
					flags |= AnimationFlags::IDLE;
					break;
				case 'P':
					flags |= AnimationFlags::INPLACE;
					break;
				default:
					break;
				}
			}

			return flags;
		}
	} // namespace mds

	static ModelScript parse_binary_script(Buffer& buf) {
		ModelScript script {};
		ModelScriptBinaryChunkType type = ModelScriptBinaryChunkType::UNKNOWN;
		int32_t ani_index = -1;

		do {
			type = static_cast<ModelScriptBinaryChunkType>(buf.get_ushort());

			auto length = buf.get_uint();
			auto chunk = buf.extract(length);

			switch (type) {
			case ModelScriptBinaryChunkType::MESH_AND_TREE: {
				MdsSkeleton mat {};
				mat.disable_mesh = chunk.get_uint() != 0;
				mat.name = chunk.get_line(false);
				script.skeleton = mat;
				break;
			}
			case ModelScriptBinaryChunkType::REGISTER_MESH:
				script.meshes.push_back(chunk.get_line());
				break;
			case ModelScriptBinaryChunkType::ANIMATION: {
				MdsAnimation anim {};
				anim.name = chunk.get_line(false);
				anim.layer = chunk.get_uint();
				anim.next = chunk.get_line(false);
				anim.blend_in = chunk.get_float();
				anim.blend_out = chunk.get_float();
				anim.flags = mds::animation_flags_from_string(chunk.get_line(false));
				anim.model = chunk.get_line(false);
				anim.direction =
				    chunk.get_line(false).find('R') == 0 ? AnimationDirection::BACKWARD : AnimationDirection::FORWARD;
				anim.first_frame = chunk.get_int();
				anim.last_frame = chunk.get_int();
				anim.fps = chunk.get_float();
				anim.speed = chunk.get_float();
				anim.collision_volume_scale = chunk.get_float();
				script.animations.push_back(std::move(anim));
				++ani_index;
				break;
			}
			case ModelScriptBinaryChunkType::ANIMATION_ALIAS: {
				MdsAnimationAlias alias {};
				alias.name = chunk.get_line(false);
				alias.layer = chunk.get_uint();
				alias.next = chunk.get_line(false);
				alias.blend_in = chunk.get_float();
				alias.blend_out = chunk.get_float();
				alias.flags = mds::animation_flags_from_string(chunk.get_line(false));
				alias.alias = chunk.get_line(false);
				alias.direction =
				    chunk.get_line(false).find('R') == 0 ? AnimationDirection::BACKWARD : AnimationDirection::FORWARD;
				script.aliases.push_back(std::move(alias));
				break;
			}
			case ModelScriptBinaryChunkType::ANIMATION_BLEND: {
				MdsAnimationBlend blend {};
				blend.name = chunk.get_line(false);
				blend.next = chunk.get_line(false);
				blend.blend_in = chunk.get_float();
				blend.blend_out = chunk.get_float();
				script.blends.push_back(std::move(blend));
				break;
			}
			case ModelScriptBinaryChunkType::ANIMATION_COMBINE: {
				MdsAnimationCombine combo {};
				combo.name = chunk.get_line(false);
				combo.layer = chunk.get_uint();
				combo.next = chunk.get_line(false);
				combo.blend_in = chunk.get_float();
				combo.blend_out = chunk.get_float();
				combo.flags = mds::animation_flags_from_string(chunk.get_line(false));
				combo.model = chunk.get_line(false);
				combo.last_frame = chunk.get_int();
				script.combinations.push_back(std::move(combo));
				break;
			}
			case ModelScriptBinaryChunkType::ANIMATION_DISABLE:
				script.disabled_animations.push_back(chunk.get_line(false));
				break;
			case ModelScriptBinaryChunkType::EVENT_CAMERA_TREMOR: {
				MdsCameraTremor trem {};
				trem.frame = chunk.get_int();
				trem.field1 = chunk.get_int();
				trem.field2 = chunk.get_int();
				trem.field3 = chunk.get_int();
				trem.field4 = chunk.get_int();
				script.animations[ani_index].tremors.push_back(std::move(trem));
				break;
			}
			case ModelScriptBinaryChunkType::EVENT_SFX: {
				MdsSoundEffect effect {};
				effect.frame = chunk.get_int();
				effect.name = chunk.get_line(false);
				effect.range = chunk.get_float();
				effect.empty_slot = chunk.get_uint() != 0;
				script.animations[ani_index].sfx.push_back(std::move(effect));
				break;
			}
			case ModelScriptBinaryChunkType::EVENT_SFX_GROUND: {
				MdsSoundEffectGround effect {};
				effect.frame = chunk.get_int();
				effect.name = chunk.get_line(false);
				effect.range = chunk.get_float();
				effect.empty_slot = chunk.get_uint() != 0;
				script.animations[ani_index].sfx_ground.push_back(std::move(effect));
				break;
			}
			case ModelScriptBinaryChunkType::MODEL_TAG: {
				MdsModelTag tag {};
				(void) chunk.get_int();

				auto event_type = chunk.get_line(false);
				if (event_type != "DEF_HIT_LIMB" && event_type != "HIT_LIMB") {
					PX_LOGW("ModelScript: unexpected type for modelTag: \"", event_type, "\"");
				}

				tag.bone = chunk.get_line(true);
				script.model_tags.push_back(std::move(tag));
				break;
			}
			case ModelScriptBinaryChunkType::EVENT_TAG: {
				MdsEventTag event {};
				event.frame = chunk.get_int();

				auto event_type = chunk.get_line(false);
				auto tp = event_types.find(event_type);
				if (tp == event_types.end()) {
					event.type = MdsEventType::UNKNOWN;
					PX_LOGW("ModelScript: unexpected value for MdsEventType: \"", event_type, "\"");
				} else {
					event.type = tp->second;
				}

				switch (event.type) {
				case MdsEventType::ITEM_CREATE:
				case MdsEventType::ITEM_EXCHANGE:
					event.slot = chunk.get_line(true);
					event.item = chunk.get_line(true);
					break;
				case MdsEventType::ITEM_INSERT:
				case MdsEventType::MUNITION_PLACE:
					event.slot = chunk.get_line(true);
					break;
				case MdsEventType::SET_FIGHT_MODE: {
					auto mode = chunk.get_line(true);

					if (mode == "FIST") {
						event.fight_mode = MdsFightMode::FIST;
					} else if (mode == "1H" || mode == "1h") {
						event.fight_mode = MdsFightMode::SINGLE_HANDED;
					} else if (mode == "2H" || mode == "2h") {
						event.fight_mode = MdsFightMode::DUAL_HANDED;
					} else if (mode == "BOW") {
						event.fight_mode = MdsFightMode::BOW;
					} else if (mode == "CBOW") {
						event.fight_mode = MdsFightMode::CROSSBOW;
					} else if (mode == "MAG") {
						event.fight_mode = MdsFightMode::MAGIC;
					} else {
						event.fight_mode = MdsFightMode::NONE;
					}
					break;
				}
				case MdsEventType::MESH_SWAP:
					event.slot = chunk.get_line(true);
					event.slot2 = chunk.get_line(true);
					break;
				case MdsEventType::HIT_LIMB:
					(void) chunk.get_line(true); // TODO
					break;
				case MdsEventType::HIT_DIRECTION:
					(void) chunk.get_line(true); // TODO
					break;
				case MdsEventType::SOUND_DRAW:
				case MdsEventType::SOUND_UNDRAW:
				case MdsEventType::MUNITION_REMOVE:
				case MdsEventType::ITEM_DESTROY:
				case MdsEventType::TORCH_INVENTORY:
				case MdsEventType::ITEM_REMOVE:
					(void) chunk.get_line(true); // TODO
					break;
				case MdsEventType::DAMAGE_MULTIPLIER:
				case MdsEventType::PARRY_FRAME:
				case MdsEventType::OPTIMAL_FRAME:
				case MdsEventType::HIT_END:
				case MdsEventType::COMBO_WINDOW: {
					auto frames = chunk.get_line(true);
					std::istringstream stream {frames};

					int32_t frame = 0;
					while (!stream.eof()) {
						stream >> frame;
						event.frames.push_back(frame);
					}
					break;
				}
				default:
					break;
				}

				script.animations[ani_index].events.push_back(std::move(event));
				break;
			}
			case ModelScriptBinaryChunkType::EVENT_PFX: {
				MdsParticleEffect effect {};
				effect.frame = chunk.get_int();
				effect.index = chunk.get_int();
				effect.name = chunk.get_line(false);
				effect.position = chunk.get_line(false);
				effect.attached = chunk.get_uint() != 0;
				script.animations[ani_index].pfx.push_back(std::move(effect));
				break;
			}
			case ModelScriptBinaryChunkType::EVENT_PFX_STOP: {
				MdsParticleEffectStop effect {};
				effect.frame = chunk.get_int();
				effect.index = chunk.get_int();
				script.animations[ani_index].pfx_stop.push_back(effect);
				break;
			}
			case ModelScriptBinaryChunkType::EVENT_MM_ANI: {
				MdsMorphAnimation anim {};
				anim.frame = chunk.get_int();
				anim.animation = chunk.get_line(false);
				anim.node = chunk.get_line(false);
				(void) chunk.get_float();
				(void) chunk.get_float();
				script.animations[ani_index].morph.push_back(std::move(anim));
				break;
			}
			case ModelScriptBinaryChunkType::ROOT:
				(void) chunk.get_uint();      // bool
				(void) chunk.get_line(false); // path
				break;
			case ModelScriptBinaryChunkType::SOURCE: {
				(void) Date::parse(chunk);    // date
				(void) chunk.get_line(false); // path
				break;
			}
			// case ModelScriptBinaryChunkType::model:
			// case ModelScriptBinaryChunkType::animation_enum:
			// case ModelScriptBinaryChunkType::animation_events_end:
			// case ModelScriptBinaryChunkType::animation_enum_end:
			// case ModelScriptBinaryChunkType::model_end:
			case ModelScriptBinaryChunkType::END:
				// empty
				break;
			case ModelScriptBinaryChunkType::ANIMATION_EVENTS:
				(void) chunk.get_uint(); // bool
				break;
			case ModelScriptBinaryChunkType::UNKNOWN:
			default:
				break;
			}

			if (chunk.remaining() != 0) {
				PX_LOGW("ModelScript: ",
				        chunk.remaining(),
				        " bytes remaining in section ",
				        std::hex,
				        std::uint16_t(type));
			}
		} while (buf.remaining() > 0);

		return script;
	}

	static ModelScript parse_source_script(Buffer& buf) {
		MdsParser p {buf.duplicate()};
		return p.parse_script();
	}

	ModelScript ModelScript::parse(Buffer& buf) {
		auto peek = buf.position();
		auto potential_chunk_type = buf.get_ushort();
		buf.position(peek);

		if (potential_chunk_type >= 0xF000 || potential_chunk_type == 0xD000) {
			return parse_binary_script(buf);
		}

		return parse_source_script(buf);
	}

	bool operator&(AnimationFlags a, AnimationFlags b) {
		return static_cast<bool>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
	}

	AnimationFlags operator|(AnimationFlags a, AnimationFlags b) {
		return static_cast<AnimationFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
	}

	AnimationFlags& operator|=(AnimationFlags& a, AnimationFlags b) {
		a = a | b;
		return a;
	}
} // namespace phoenix
