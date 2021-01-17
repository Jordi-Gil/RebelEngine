#include "ComponentLight.h"

#include "Math/TransformOps.h"
#include "Math/float3x3.h"

#include "ImGui/imgui_utils.h"
#include "ImGui/IconsFontAwesome5.h"
#include "ImGui/IconsForkAwesome.h"

#include "Main/GameObject.h"

#include "Utils/debugdraw.h"
#include <ImGui/imgui_internal.h>

static const char* lightTypes[3] = { "Directional Light", "Point Light", "Spot Light" };

ComponentLight::ComponentLight() {
	_type = type_component::LIGHT;
}

ComponentLight::ComponentLight(light_type type, float intensity, float range, float angle) {

	_type = type_component::LIGHT;
	_light_type = type;
	_intensity = intensity;
	_range = range;
	_spot_angle = angle;

}

ComponentLight::ComponentLight(const ComponentLight& copy) {

	_owner = copy._owner;
	_active = copy._active;
	_type = copy._type;
	_uuid = copy._uuid;

	_intensity = copy._intensity;
	_range = copy._range;
	_spot_angle = copy._spot_angle;
	_light_type = copy._light_type;

}

ComponentLight::ComponentLight(const Json::Value& value) {
	Component::FromJson(value);
	_type = type_component::LIGHT;
	FromJson(value);
}

void ComponentLight::SetIntensity(float intensity) {
	_intensity = intensity;
}

void ComponentLight::SetRange(float range) {
	_range = range;
}

void ComponentLight::SetSpotAngle(float angle) {
	_spot_angle = angle;
}

void ComponentLight::SetColor(const float3& color) {
	_color = color;
}

void ComponentLight::ChangeType(light_type newType) {
	_light_type = newType;
}

bool ComponentLight::ToJson(Json::Value& value, int pos){
	
	Component::ToJson(value, pos);
	value[pos][JSON_TAG_LIGHT_INTENSITY] = _intensity;
	value[pos][JSON_TAG_LIGHT_RANGE] = _range;
	value[pos][JSON_TAG_LIGHT_ANGLE] = _spot_angle;
	value[pos][JSON_TAG_LIGHT_TYPE] = (int)_light_type;
	value[pos][JSON_TAG_LIGHT_CONSTANT_ATT] = _constant_att;
	value[pos][JSON_TAG_LIGHT_LINEAR_ATT] = _linear_att;
	value[pos][JSON_TAG_LIGHT_QUADRATIC_ATT] = _quadratic_att;

	Json::Value arr(Json::arrayValue);
	arr.append(_color.x); arr.append(_color.y); arr.append(_color.z);
	value[pos][JSON_TAG_LIGHT_COLOR] = arr;

	return true;
}

bool ComponentLight::FromJson(const Json::Value& value){

	if (!value.isNull()) {

		_intensity = value[JSON_TAG_LIGHT_INTENSITY].asFloat();
		_range = value[JSON_TAG_LIGHT_RANGE].asFloat();
		_spot_angle = value[JSON_TAG_LIGHT_ANGLE].asFloat();
		_constant_att = value[JSON_TAG_LIGHT_CONSTANT_ATT].asFloat();
		_linear_att = value[JSON_TAG_LIGHT_LINEAR_ATT].asFloat();
		_quadratic_att = value[JSON_TAG_LIGHT_QUADRATIC_ATT].asFloat();
		_light_type = (light_type) value[JSON_TAG_TYPE].asInt();
		_color = float3(
				value[JSON_TAG_LIGHT_COLOR][0].asFloat(),
				value[JSON_TAG_LIGHT_COLOR][1].asFloat(),
				value[JSON_TAG_LIGHT_COLOR][2].asFloat()
		);

		return true;
	}
	else return false;

}

void ComponentLight::DebugDraw() {

	if(_owner->IsSelected()){
		switch (_light_type) {

			case SPOT_LIGHT: {
				float3x3 rot = (_owner->GetGlobalMatrix()).RotatePart();
				float3 position = (_owner->GetGlobalMatrix()).TranslatePart();
				float3 forward = rot.Col(2);

				dd::cone(position, -forward, dd::colors::Yellow, 1, 0.01);
				break;
			}

			case POINT_LIGHT: {

				float3 position = _owner->GetGlobalMatrix().TranslatePart();
				dd::sphere(position, dd::colors::Yellow, _range);
				break;

			}

			case DIRECTIONAL_LIGHT: {
				float3 forward = (_owner->GetGlobalMatrix()).RotatePart().Col(2);
				float3 position = _owner->GetGlobalMatrix().TranslatePart();

				for (int i = 0; i < 10; i++) {

					float theta = (2.0f * pi * float(i)) / float(10);//get the current angle

					float3 p00 = position;
					float x = 0.15f * cosf(theta);//calculate the x component
					float y = 0.15f * sinf(theta);//calculate the y component
					p00.x += x; p00.y += y;
					float3 p01 = position + forward;
					p01.x += x; p01.y += y;

					dd::arrow(p00, p01, dd::colors::Yellow, 0.01f);

				}
			}
		}
	}
}

void ComponentLight::OnEditor() {

	static int current = 0;

	if (ImGui::CollapsingHeader(ICON_FA_LIGHTBULB " Light", ImGuiTreeNodeFlags_DefaultOpen)) {

		std::string id;

		ImGui::BeginColumns("##light",2, ImGuiColumnsFlags_NoBorder | ImGuiColumnsFlags_NoResize);
		
		ImGui::Text("Type");
		ImGui::Text("Color");
		ImGui::Text("Intensity");
		if (_light_type == POINT_LIGHT) {
			ImGui::Text("Attenuation");
			ImGui::Text("Constant");
			ImGui::Text("Linear");
			ImGui::Text("Quadratic");
		}

		ImGui::NextColumn();

		id = "##type_"; id.append(_uuid);
		ImGui::PushID(id.c_str());
		ImGui::Combo("##", &current, lightTypes, IM_ARRAYSIZE(lightTypes));
		ImGui::PopID();

		id = "##color_"; id.append(_uuid);
		ImGui::PushID(id.c_str());
		ImGui::ColorEdit4("", &_color[0], ImGuiColorEditFlags_NoInputs);		
		ImGui::PopID();

		id = "##intensity_"; id.append(_uuid);
		ImGui::PushID(id.c_str());
		ImGui::DragFloat("", &_intensity, 0.01f, 0, 500, "%.2f");
		ImGui::PopID();

		if (_light_type == POINT_LIGHT) {
			ImGui::NewLine();
			
			id = "##constant_"; id.append(_uuid);
			ImGui::PushID(id.c_str());
			ImGui::Text("%s",_constant_att);
			ImGui::PopID();

			id = "##linear_"; id.append(_uuid);
			ImGui::PushID(id.c_str());
			ImGui::DragFloat("", &_linear_att, 0.01f, 0, 1, "%.2f");
			ImGui::PopID();

			id = "##constant_"; id.append(_uuid);
			ImGui::PushID(id.c_str());
			ImGui::DragFloat("", &_quadratic_att, 0.01f, 0, 1, "%.2f");
			ImGui::PopID();
		}

		ImGui::EndColumns();

	}

}