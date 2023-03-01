// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/vobs/vob.hh"
#include "phoenix/archive.hh"

#include <unordered_map>

namespace phoenix {
	/// \brief A mapping of archive class names to visual_type values.
	static std::unordered_map<std::string, VisualType> visual_type_map = {
	    {"zCDecal", VisualType::DECAL},
	    {"zCMesh", VisualType::MESH},
	    {"zCProgMeshProto", VisualType::MULTI_RESOLUTION_MESH},
	    {"zCParticleFX", VisualType::PARTICLE_EFFECT},
	    {"zCModel", VisualType::MODEL},
	    {"zCAICamera", VisualType::AI_CAMERA},
	    {"zCMorphMesh", VisualType::MORPH_MESH},
	    {"\xA7", VisualType::UNKNOWN},
	    {"%", VisualType::UNKNOWN},
	};

	Decal Decal::parse(ArchiveReader& in, GameVersion version) {
		Decal dc {};
		dc.name = in.read_string();                                 // name
		dc.dimension = in.read_vec2();                              // decalDim
		dc.offset = in.read_vec2();                                 // decalOffset
		dc.two_sided = in.read_bool();                              // decal2Sided
		dc.alpha_func = static_cast<AlphaFunction>(in.read_enum()); // decalAlphaFunc
		dc.texture_anim_fps = in.read_float();                      // decalTexAniFPS

		if (version == GameVersion::GOTHIC_2) {
			dc.alpha_weight = in.read_byte();    // decalAlphaWeight
			dc.ignore_daylight = in.read_bool(); // ignoreDayLight
		}

		return dc;
	}

	void VirtualObject::parse(VirtualObject& obj, ArchiveReader& in, GameVersion version) {
		auto packed = in.read_int() != 0; // pack
		bool has_visual_object = true;
		bool has_ai_object = true;
		bool has_event_manager_object = false;

		if (packed) {
			auto bin = in.read_raw_bytes(version == GameVersion::GOTHIC_1 ? 74 : 83); // dataRaw

			obj.bbox = AxisAlignedBoundingBox::parse(bin);
			obj.position = bin.get_vec3();
			obj.rotation = bin.get_mat3x3();

			std::uint8_t bit0 = bin.get();
			std::uint16_t bit1;

			if (version == GameVersion::GOTHIC_1) {
				bit1 = bin.get();
			} else {
				bit1 = bin.get_ushort();
			}

			obj.show_visual = static_cast<bool>((bit0 & 0b00000001) >> 0);
			obj.sprite_camera_facing_mode = static_cast<SpriteAlignment>((bit0 & 0b00000110) >> 1);
			obj.cd_static = static_cast<bool>((bit0 & 0b00001000) >> 3);
			obj.cd_dynamic = static_cast<bool>((bit0 & 0b00010000) >> 4);
			obj.vob_static = static_cast<bool>((bit0 & 0b00100000) >> 5);
			obj.dynamic_shadows = static_cast<ShadowType>((bit0 & 0b11000000) >> 6);

			bool has_preset_name = static_cast<bool>((bit1 & 0b000000000000001u) >> 0u);
			bool has_vob_name = static_cast<bool>((bit1 & 0b000000000000010u) >> 1u);
			bool has_visual_name = static_cast<bool>((bit1 & 0b000000000000100u) >> 2u);
			has_visual_object = static_cast<bool>((bit1 & 0b000000000001000u) >> 3u);
			has_ai_object = static_cast<bool>((bit1 & 0b000000000010000u) >> 4u);

			// Quirk: bit 5 of this bitfield specifies whether an event manger object is
			// present but this is only relevant in save-games.
			has_event_manager_object = static_cast<bool>((bit1 & 0b000000000100000u) >> 5u) && in.get_header().save;

			obj.physics_enabled = static_cast<bool>((bit1 & 0b000000001000000u) >> 6u);

			if (version == GameVersion::GOTHIC_2) {
				obj.anim_mode = static_cast<AnimationType>(bit1 & 0b000000110000000u >> 7u);
				obj.bias = static_cast<std::int32_t>((bit1 & 0b011111000000000u) >> 9u);
				obj.ambient = static_cast<bool>((bit1 & 0b100000000000000u) >> 14u);

				obj.anim_strength = bin.get_float();
				obj.far_clip_scale = bin.get_float();
			}

			if (has_preset_name) {
				obj.preset_name = in.read_string(); // presetName
			}

			if (has_vob_name) {
				obj.vob_name = in.read_string(); // vobName
			}

			if (has_visual_name) {
				obj.visual_name = in.read_string(); // visual
			}
		} else {
			obj.preset_name = in.read_string();
			obj.bbox = in.read_bbox(); // bbox3DWS

			obj.rotation = in.read_mat3x3(); // trafoOSToWSRot
			obj.position = in.read_vec3();   // trafoOSToWSPos

			obj.vob_name = in.read_string();                                              // vobName
			obj.visual_name = in.read_string();                                           // visual
			obj.show_visual = in.read_bool();                                             // showVisual
			obj.sprite_camera_facing_mode = static_cast<SpriteAlignment>(in.read_enum()); // visualCamAlign

			if (version == GameVersion::GOTHIC_1) {
				obj.cd_static = in.read_bool();                                // cdStatic
				obj.cd_dynamic = in.read_bool();                               // cdDyn
				obj.vob_static = in.read_bool();                               // staticVob
				obj.dynamic_shadows = static_cast<ShadowType>(in.read_enum()); // dynShadow
			} else {
				obj.anim_mode = static_cast<AnimationType>(in.read_enum());    // visualAniMode
				obj.anim_strength = in.read_float();                           // visualAniModeStrength
				obj.far_clip_scale = in.read_float();                          // vobFarClipZScale
				obj.cd_static = in.read_bool();                                // cdStatic
				obj.cd_dynamic = in.read_bool();                               // cdDyn
				obj.vob_static = in.read_bool();                               // staticVob
				obj.dynamic_shadows = static_cast<ShadowType>(in.read_enum()); // dynShadow
				obj.bias = in.read_int();                                      // zbias
				obj.ambient = in.read_bool();                                  // isAmbient
			}
		}

		if (has_visual_object) {
			ArchiveObject visual {};
			in.read_object_begin(visual);
			obj.associated_visual_type = visual_type_map[visual.class_name];

			if (obj.associated_visual_type == VisualType::DECAL) {
				obj.visual_decal = Decal::parse(in, version);
			}

			if (!in.read_object_end()) {
				PX_LOGW("vob_tree: visual \"", visual.class_name, "\" not fully parsed");
				in.skip_object(true);
			}
		}

		// TODO
		if (has_ai_object) {
			in.skip_object(false);
		}

		// TODO
		if (has_event_manager_object) {
			in.skip_object(false);
		}

		if (in.get_header().save) {
			// save-games contain two extra values for each VOb
			obj.saved = SaveState {};
			obj.saved->sleep_mode = in.read_byte();     // sleepMode
			obj.saved->next_on_timer = in.read_float(); // nextOnTimer
		}
	}
} // namespace phoenix
