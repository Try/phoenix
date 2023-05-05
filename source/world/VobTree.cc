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
	static std::unordered_map<std::string, VobType> VobType_map = {
	    {"zCVob", VobType::zCVob},
	    {"zCVobLevelCompo:zCVob", VobType::zCVobLevelCompo},
	    {"oCItem:zCVob", VobType::oCItem},
	    {"oCNpc:zCVob", VobType::oCNpc},
	    {"oCMOB:zCVob", VobType::oCMOB},
	    {"oCMobInter:oCMOB:zCVob", VobType::oCMobInter},
	    {"oCMobBed:oCMobInter:oCMOB:zCVob", VobType::oCMobBed},
	    {"oCMobFire:oCMobInter:oCMOB:zCVob", VobType::oCMobFire},
	    {"oCMobLadder:oCMobInter:oCMOB:zCVob", VobType::oCMobLadder},
	    {"oCMobSwitch:oCMobInter:oCMOB:zCVob", VobType::oCMobSwitch},
	    {"oCMobWheel:oCMobInter:oCMOB:zCVob", VobType::oCMobWheel},
	    {"oCMobContainer:oCMobInter:oCMOB:zCVob", VobType::oCMobContainer},
	    {"oCMobDoor:oCMobInter:oCMOB:zCVob", VobType::oCMobDoor},
	    {"zCPFXControler:zCVob", VobType::zCPFXController},
	    {"zCVobAnimate:zCVob", VobType::zCVobAnimate},
	    {"zCVobLensFlare:zCVob", VobType::zCVobLensFlare},
	    {"zCVobLight:zCVob", VobType::zCVobLight},
	    {"zCVobSpot:zCVob", VobType::zCVobSpot},
	    {"zCVobStartpoint:zCVob", VobType::zCVobStartpoint},
	    {"zCVobSound:zCVob", VobType::zCVobSound},
	    {"zCVobSoundDaytime:zCVobSound:zCVob", VobType::zCVobSoundDaytime},
	    {"oCZoneMusic:zCVob", VobType::oCZoneMusic},
	    {"oCZoneMusicDefault:oCZoneMusic:zCVob", VobType::oCZoneMusicDefault},
	    {"zCZoneZFog:zCVob", VobType::zCZoneZFog},
	    {"zCZoneZFogDefault:zCZoneZFog:zCVob", VobType::zCZoneZFogDefault},
	    {"zCZoneVobFarPlane:zCVob", VobType::zCZoneVobFarPlane},
	    {"zCZoneVobFarPlaneDefault:zCZoneVobFarPlane:zCVob", VobType::zCZoneVobFarPlaneDefault},
	    {"zCMessageFilter:zCVob", VobType::zCMessageFilter},
	    {"zCCodeMaster:zCVob", VobType::zCCodeMaster},
	    {"zCTrigger:zCVob", VobType::zCTrigger},
	    {"zCTriggerList:zCTrigger:zCVob", VobType::zCTriggerList},
	    {"oCTriggerScript:zCTrigger:zCVob", VobType::oCTriggerScript},
	    {"zCMover:zCTrigger:zCVob", VobType::zCMover},
	    {"oCTriggerChangeLevel:zCTrigger:zCVob", VobType::oCTriggerChangeLevel},
	    {"zCTriggerWorldStart:zCVob", VobType::zCTriggerWorldStart},
	    {"zCTriggerUntouch:zCVob", VobType::zCTriggerUntouch},
	    {"zCCSCamera:zCVob", VobType::zCCSCamera},
	    {"zCCamTrj_KeyFrame:zCVob", VobType::zCCamTrj_KeyFrame},
	    {"oCTouchDamage:zCTouchDamage:zCVob", VobType::oCTouchDamage},
	    {"zCEarthquake:zCVob", VobType::zCEarthquake},
	    {"zCMoverControler:zCVob", VobType::zCMoverController},
	    {"zCVobScreenFX:zCVob", VobType::zCVobScreenFX},
	    {"zCVobStair:zCVob", VobType::zCVobStair},
	    {"oCCSTrigger:zCTrigger:zCVob", VobType::oCCSTrigger},
	    {"\xA7", VobType::ignored}, // some sort of padding object, probably. seems to be always empty
	};

	std::unique_ptr<VirtualObject> parse_vob_tree(ArchiveReader& in, GameVersion version) {
		std::vector<std::unique_ptr<VirtualObject>> vobs {};

		ArchiveObject obj;
		if (!in.read_object_begin(obj)) {
			throw ParserError {"vob_tree", "expected object where there was none"};
		}

		VobType type;

		if (const auto& it = VobType_map.find(obj.class_name); it != VobType_map.end()) {
			type = it->second;
		} else {
			type = VobType::unknown;
		}

		std::unique_ptr<VirtualObject> object;

		switch (type) {
		case VobType::zCCamTrj_KeyFrame:
		case VobType::zCVobLevelCompo:
		case VobType::zCVobStartpoint:
		case VobType::zCVobScreenFX:
		case VobType::zCVobStair:
		case VobType::zCVobSpot:
		case VobType::zCVob:
			object = std::make_unique<VirtualObject>();
			VirtualObject::parse(*object, in, version);
			break;
		case VobType::zCCSCamera:
			object = std::make_unique<vobs::CutsceneCamera>();
			vobs::CutsceneCamera::parse(dynamic_cast<vobs::CutsceneCamera&>(*object), in, version);
			break;
		case VobType::zCVobAnimate:
			object = std::make_unique<vobs::Animate>();
			vobs::Animate::parse(dynamic_cast<vobs::Animate&>(*object), in, version);
			break;
		case VobType::zCZoneVobFarPlane:
		case VobType::zCZoneVobFarPlaneDefault:
			object = std::make_unique<vobs::ZoneFarPlane>();
			vobs::ZoneFarPlane::parse(dynamic_cast<vobs::ZoneFarPlane&>(*object), in, version);
			break;
		case VobType::zCZoneZFogDefault:
		case VobType::zCZoneZFog:
			object = std::make_unique<vobs::ZoneFog>();
			vobs::ZoneFog::parse(dynamic_cast<vobs::ZoneFog&>(*object), in, version);
			break;
		case VobType::zCVobLensFlare:
			object = std::make_unique<vobs::LensFlare>();
			vobs::LensFlare::parse(dynamic_cast<vobs::LensFlare&>(*object), in, version);
			break;
		case VobType::oCItem:
			object = std::make_unique<vobs::Item>();
			vobs::Item::parse(dynamic_cast<vobs::Item&>(*object), in, version);
			break;
		case VobType::zCTrigger:
		case VobType::oCCSTrigger:
			object = std::make_unique<vobs::Trigger>();
			vobs::Trigger::parse(dynamic_cast<vobs::Trigger&>(*object), in, version);
			break;
		case VobType::oCMOB:
			object = std::make_unique<vobs::MovableObject>();
			vobs::MovableObject::parse(dynamic_cast<vobs::MovableObject&>(*object), in, version);
			break;
		case VobType::oCMobInter:
		case VobType::oCMobLadder:
		case VobType::oCMobSwitch:
		case VobType::oCMobWheel:
		case VobType::oCMobBed:
			object = std::make_unique<vobs::InteractiveObject>();
			vobs::InteractiveObject::parse(dynamic_cast<vobs::InteractiveObject&>(*object), in, version);
			break;
		case VobType::oCMobFire:
			object = std::make_unique<vobs::Fire>();
			vobs::Fire::parse(dynamic_cast<vobs::Fire&>(*object), in, version);
			break;
		case VobType::oCMobContainer:
			object = std::make_unique<vobs::Container>();
			vobs::Container::parse(dynamic_cast<vobs::Container&>(*object), in, version);
			break;
		case VobType::oCMobDoor:
			object = std::make_unique<vobs::Door>();
			vobs::Door::parse(dynamic_cast<vobs::Door&>(*object), in, version);
			break;
		case VobType::zCPFXController:
			object = std::make_unique<vobs::ParticleEffectController>();
			vobs::ParticleEffectController::parse(dynamic_cast<vobs::ParticleEffectController&>(*object), in, version);
			break;
		case VobType::zCVobLight:
			object = std::make_unique<vobs::Light>();
			vobs::Light::parse((vobs::Light&) *object, in, version);
			break;
		case VobType::zCVobSound:
			object = std::make_unique<vobs::Sound>();
			vobs::Sound::parse(dynamic_cast<vobs::Sound&>(*object), in, version);
			break;
		case VobType::zCVobSoundDaytime:
			object = std::make_unique<vobs::SoundDaytime>();
			vobs::SoundDaytime::parse(dynamic_cast<vobs::SoundDaytime&>(*object), in, version);
			break;
		case VobType::oCZoneMusic:
		case VobType::oCZoneMusicDefault:
			object = std::make_unique<vobs::ZoneMusic>();
			vobs::ZoneMusic::parse(dynamic_cast<vobs::ZoneMusic&>(*object), in, version);
			break;
		case VobType::zCMessageFilter:
			object = std::make_unique<vobs::MessageFilter>();
			vobs::MessageFilter::parse(dynamic_cast<vobs::MessageFilter&>(*object), in, version);
			break;
		case VobType::zCCodeMaster:
			object = std::make_unique<vobs::CodeMaster>();
			vobs::CodeMaster::parse(dynamic_cast<vobs::CodeMaster&>(*object), in, version);
			break;
		case VobType::zCTriggerList:
			object = std::make_unique<vobs::TriggerList>();
			vobs::TriggerList::parse(dynamic_cast<vobs::TriggerList&>(*object), in, version);
			break;
		case VobType::oCTriggerScript:
			object = std::make_unique<vobs::TriggerScript>();
			vobs::TriggerScript::parse(dynamic_cast<vobs::TriggerScript&>(*object), in, version);
			break;
		case VobType::zCMover:
			object = std::make_unique<vobs::Mover>();
			vobs::Mover::parse(dynamic_cast<vobs::Mover&>(*object), in, version);
			break;
		case VobType::oCTriggerChangeLevel:
			object = std::make_unique<vobs::TriggerChangeLevel>();
			vobs::TriggerChangeLevel::parse(dynamic_cast<vobs::TriggerChangeLevel&>(*object), in, version);
			break;
		case VobType::zCTriggerWorldStart:
			object = std::make_unique<vobs::TriggerWorldStart>();
			vobs::TriggerWorldStart::parse(dynamic_cast<vobs::TriggerWorldStart&>(*object), in, version);
			break;
		case VobType::oCTouchDamage:
			object = std::make_unique<vobs::TouchDamage>();
			vobs::TouchDamage::parse(dynamic_cast<vobs::TouchDamage&>(*object), in, version);
			break;
		case VobType::zCTriggerUntouch:
			object = std::make_unique<vobs::TriggerUntouch>();
			vobs::TriggerUntouch::parse(dynamic_cast<vobs::TriggerUntouch&>(*object), in, version);
			break;
		case VobType::zCEarthquake:
			object = std::make_unique<vobs::Earthquake>();
			vobs::Earthquake::parse(dynamic_cast<vobs::Earthquake&>(*object), in, version);
			break;
		case VobType::zCMoverController:
			object = std::make_unique<vobs::MoverController>();
			vobs::MoverController::parse(dynamic_cast<vobs::MoverController&>(*object), in, version);
			break;
		case VobType::oCNpc:
			object = std::make_unique<vobs::Npc>();
			vobs::Npc::parse(dynamic_cast<vobs::Npc&>(*object), in, version);
			break;
		case VobType::ignored:
			break;
		case VobType::unknown:
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
