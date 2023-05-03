// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/world/VobTree.hh"
#include "phoenix/Archive.hh"
#include "phoenix/vobs/Camera.hh"
#include "phoenix/vobs/Light.hh"
#include "phoenix/vobs/Misc.hh"
#include "phoenix/vobs/MovableObject.hh"
#include "phoenix/vobs/Sound.hh"
#include "phoenix/vobs/Trigger.hh"
#include "phoenix/vobs/VirtualObject.hh"
#include "phoenix/vobs/Zone.hh"

#include <unordered_map>

namespace phoenix {
	static std::unordered_map<std::string, vob_type> vob_type_map = {
	    {"zCVob", vob_type::zCVob},
	    {"zCVobLevelCompo:zCVob", vob_type::zCVobLevelCompo},
	    {"oCItem:zCVob", vob_type::oCItem},
	    {"oCNpc:zCVob", vob_type::oCNpc},
	    {"oCMOB:zCVob", vob_type::oCMOB},
	    {"oCMobInter:oCMOB:zCVob", vob_type::oCMobInter},
	    {"oCMobBed:oCMobInter:oCMOB:zCVob", vob_type::oCMobBed},
	    {"oCMobFire:oCMobInter:oCMOB:zCVob", vob_type::oCMobFire},
	    {"oCMobLadder:oCMobInter:oCMOB:zCVob", vob_type::oCMobLadder},
	    {"oCMobSwitch:oCMobInter:oCMOB:zCVob", vob_type::oCMobSwitch},
	    {"oCMobWheel:oCMobInter:oCMOB:zCVob", vob_type::oCMobWheel},
	    {"oCMobContainer:oCMobInter:oCMOB:zCVob", vob_type::oCMobContainer},
	    {"oCMobDoor:oCMobInter:oCMOB:zCVob", vob_type::oCMobDoor},
	    {"zCPFXControler:zCVob", vob_type::zCPFXController},
	    {"zCVobAnimate:zCVob", vob_type::zCVobAnimate},
	    {"zCVobLensFlare:zCVob", vob_type::zCVobLensFlare},
	    {"zCVobLight:zCVob", vob_type::zCVobLight},
	    {"zCVobSpot:zCVob", vob_type::zCVobSpot},
	    {"zCVobStartpoint:zCVob", vob_type::zCVobStartpoint},
	    {"zCVobSound:zCVob", vob_type::zCVobSound},
	    {"zCVobSoundDaytime:zCVobSound:zCVob", vob_type::zCVobSoundDaytime},
	    {"oCZoneMusic:zCVob", vob_type::oCZoneMusic},
	    {"oCZoneMusicDefault:oCZoneMusic:zCVob", vob_type::oCZoneMusicDefault},
	    {"zCZoneZFog:zCVob", vob_type::zCZoneZFog},
	    {"zCZoneZFogDefault:zCZoneZFog:zCVob", vob_type::zCZoneZFogDefault},
	    {"zCZoneVobFarPlane:zCVob", vob_type::zCZoneVobFarPlane},
	    {"zCZoneVobFarPlaneDefault:zCZoneVobFarPlane:zCVob", vob_type::zCZoneVobFarPlaneDefault},
	    {"zCMessageFilter:zCVob", vob_type::zCMessageFilter},
	    {"zCCodeMaster:zCVob", vob_type::zCCodeMaster},
	    {"zCTrigger:zCVob", vob_type::zCTrigger},
	    {"zCTriggerList:zCTrigger:zCVob", vob_type::zCTriggerList},
	    {"oCTriggerScript:zCTrigger:zCVob", vob_type::oCTriggerScript},
	    {"zCMover:zCTrigger:zCVob", vob_type::zCMover},
	    {"oCTriggerChangeLevel:zCTrigger:zCVob", vob_type::oCTriggerChangeLevel},
	    {"zCTriggerWorldStart:zCVob", vob_type::zCTriggerWorldStart},
	    {"zCTriggerUntouch:zCVob", vob_type::zCTriggerUntouch},
	    {"zCCSCamera:zCVob", vob_type::zCCSCamera},
	    {"zCCamTrj_KeyFrame:zCVob", vob_type::zCCamTrj_KeyFrame},
	    {"oCTouchDamage:zCTouchDamage:zCVob", vob_type::oCTouchDamage},
	    {"zCEarthquake:zCVob", vob_type::zCEarthquake},
	    {"zCMoverControler:zCVob", vob_type::zCMoverController},
	    {"zCVobScreenFX:zCVob", vob_type::zCVobScreenFX},
	    {"zCVobStair:zCVob", vob_type::zCVobStair},
	    {"oCCSTrigger:zCTrigger:zCVob", vob_type::oCCSTrigger},
	    {"\xA7", vob_type::ignored}, // some sort of padding object, probably. seems to be always empty
	};

	std::unique_ptr<VirtualObject> parse_vob_tree(ArchiveReader& in, GameVersion version) {
		std::vector<std::unique_ptr<VirtualObject>> vobs {};

		ArchiveObject obj;
		if (!in.read_object_begin(obj)) {
			throw ParserError {"vob_tree", "expected object where there was none"};
		}

		vob_type type;

		if (const auto& it = vob_type_map.find(obj.class_name); it != vob_type_map.end()) {
			type = it->second;
		} else {
			type = vob_type::unknown;
		}

		std::unique_ptr<VirtualObject> object;

		switch (type) {
		case vob_type::zCCamTrj_KeyFrame:
		case vob_type::zCVobLevelCompo:
		case vob_type::zCVobStartpoint:
		case vob_type::zCVobScreenFX:
		case vob_type::zCVobStair:
		case vob_type::zCVobSpot:
		case vob_type::zCVob:
			object = std::make_unique<VirtualObject>();
			VirtualObject::parse(*object, in, version);
			break;
		case vob_type::zCCSCamera:
			object = std::make_unique<vobs::CutsceneCamera>();
			vobs::CutsceneCamera::parse(dynamic_cast<vobs::CutsceneCamera&>(*object), in, version);
			break;
		case vob_type::zCVobAnimate:
			object = std::make_unique<vobs::Animate>();
			vobs::Animate::parse(dynamic_cast<vobs::Animate&>(*object), in, version);
			break;
		case vob_type::zCZoneVobFarPlane:
		case vob_type::zCZoneVobFarPlaneDefault:
			object = std::make_unique<vobs::ZoneFarPlane>();
			vobs::ZoneFarPlane::parse(dynamic_cast<vobs::ZoneFarPlane&>(*object), in, version);
			break;
		case vob_type::zCZoneZFogDefault:
		case vob_type::zCZoneZFog:
			object = std::make_unique<vobs::ZoneFog>();
			vobs::ZoneFog::parse(dynamic_cast<vobs::ZoneFog&>(*object), in, version);
			break;
		case vob_type::zCVobLensFlare:
			object = std::make_unique<vobs::LensFlare>();
			vobs::LensFlare::parse(dynamic_cast<vobs::LensFlare&>(*object), in, version);
			break;
		case vob_type::oCItem:
			object = std::make_unique<vobs::Item>();
			vobs::Item::parse(dynamic_cast<vobs::Item&>(*object), in, version);
			break;
		case vob_type::zCTrigger:
		case vob_type::oCCSTrigger:
			object = std::make_unique<vobs::Trigger>();
			vobs::Trigger::parse(dynamic_cast<vobs::Trigger&>(*object), in, version);
			break;
		case vob_type::oCMOB:
			object = std::make_unique<vobs::MovableObject>();
			vobs::MovableObject::parse(dynamic_cast<vobs::MovableObject&>(*object), in, version);
			break;
		case vob_type::oCMobInter:
		case vob_type::oCMobLadder:
		case vob_type::oCMobSwitch:
		case vob_type::oCMobWheel:
		case vob_type::oCMobBed:
			object = std::make_unique<vobs::InteractiveObject>();
			vobs::InteractiveObject::parse(dynamic_cast<vobs::InteractiveObject&>(*object), in, version);
			break;
		case vob_type::oCMobFire:
			object = std::make_unique<vobs::Fire>();
			vobs::Fire::parse(dynamic_cast<vobs::Fire&>(*object), in, version);
			break;
		case vob_type::oCMobContainer:
			object = std::make_unique<vobs::Container>();
			vobs::Container::parse(dynamic_cast<vobs::Container&>(*object), in, version);
			break;
		case vob_type::oCMobDoor:
			object = std::make_unique<vobs::Door>();
			vobs::Door::parse(dynamic_cast<vobs::Door&>(*object), in, version);
			break;
		case vob_type::zCPFXController:
			object = std::make_unique<vobs::ParticleEffectController>();
			vobs::ParticleEffectController::parse(dynamic_cast<vobs::ParticleEffectController&>(*object), in, version);
			break;
		case vob_type::zCVobLight:
			object = std::make_unique<vobs::Light>();
			vobs::Light::parse((vobs::Light&) *object, in, version);
			break;
		case vob_type::zCVobSound:
			object = std::make_unique<vobs::Sound>();
			vobs::Sound::parse(dynamic_cast<vobs::Sound&>(*object), in, version);
			break;
		case vob_type::zCVobSoundDaytime:
			object = std::make_unique<vobs::SoundDaytime>();
			vobs::SoundDaytime::parse(dynamic_cast<vobs::SoundDaytime&>(*object), in, version);
			break;
		case vob_type::oCZoneMusic:
		case vob_type::oCZoneMusicDefault:
			object = std::make_unique<vobs::ZoneMusic>();
			vobs::ZoneMusic::parse(dynamic_cast<vobs::ZoneMusic&>(*object), in, version);
			break;
		case vob_type::zCMessageFilter:
			object = std::make_unique<vobs::MessageFilter>();
			vobs::MessageFilter::parse(dynamic_cast<vobs::MessageFilter&>(*object), in, version);
			break;
		case vob_type::zCCodeMaster:
			object = std::make_unique<vobs::CodeMaster>();
			vobs::CodeMaster::parse(dynamic_cast<vobs::CodeMaster&>(*object), in, version);
			break;
		case vob_type::zCTriggerList:
			object = std::make_unique<vobs::TriggerList>();
			vobs::TriggerList::parse(dynamic_cast<vobs::TriggerList&>(*object), in, version);
			break;
		case vob_type::oCTriggerScript:
			object = std::make_unique<vobs::TriggerScript>();
			vobs::TriggerScript::parse(dynamic_cast<vobs::TriggerScript&>(*object), in, version);
			break;
		case vob_type::zCMover:
			object = std::make_unique<vobs::Mover>();
			vobs::Mover::parse(dynamic_cast<vobs::Mover&>(*object), in, version);
			break;
		case vob_type::oCTriggerChangeLevel:
			object = std::make_unique<vobs::TriggerChangeLevel>();
			vobs::TriggerChangeLevel::parse(dynamic_cast<vobs::TriggerChangeLevel&>(*object), in, version);
			break;
		case vob_type::zCTriggerWorldStart:
			object = std::make_unique<vobs::TriggerWorldStart>();
			vobs::TriggerWorldStart::parse(dynamic_cast<vobs::TriggerWorldStart&>(*object), in, version);
			break;
		case vob_type::oCTouchDamage:
			object = std::make_unique<vobs::TouchDamage>();
			vobs::TouchDamage::parse(dynamic_cast<vobs::TouchDamage&>(*object), in, version);
			break;
		case vob_type::zCTriggerUntouch:
			object = std::make_unique<vobs::TriggerUntouch>();
			vobs::TriggerUntouch::parse(dynamic_cast<vobs::TriggerUntouch&>(*object), in, version);
			break;
		case vob_type::zCEarthquake:
			object = std::make_unique<vobs::Earthquake>();
			vobs::Earthquake::parse(dynamic_cast<vobs::Earthquake&>(*object), in, version);
			break;
		case vob_type::zCMoverController:
			object = std::make_unique<vobs::MoverController>();
			vobs::MoverController::parse(dynamic_cast<vobs::MoverController&>(*object), in, version);
			break;
		case vob_type::oCNpc:
			object = std::make_unique<vobs::Npc>();
			vobs::Npc::parse(dynamic_cast<vobs::Npc&>(*object), in, version);
			break;
		case vob_type::ignored:
			break;
		case vob_type::unknown:
			PX_LOGW("vob_tree: encountered unknown VOb [",
			        obj.object_name,
			        " ",
			        obj.class_name,
			        " ",
			        obj.version,
			        " ",
			        obj.index,
			        "]");
			break;
		}

		if (!in.read_object_end()) {
			PX_LOGW("VirtualObject: VOb \"", obj.class_name, "\" not fully parsed");
			in.skip_object(true);
		}

		auto child_count = in.read_int();
		if (object == nullptr) {
			std::function<void(int32_t)> skip;
			skip = [&skip, &in](int32_t count) {
				for (int32_t i = 0; i < count; ++i) {
					in.skip_object(false);

					auto num_children = in.read_int();
					skip(num_children);
				}
			};

			skip(child_count);
			return nullptr;
		}

		object->children.reserve(child_count);
		object->id = obj.index;
		object->type = type;

		for (int32_t i = 0; i < child_count; ++i) {
			auto child = parse_vob_tree(in, version);
			if (child == nullptr)
				continue;

			object->children.push_back(std::move(child));
		}

		return object;
	}

} // namespace phoenix
