#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

#include "TextureObject.h"
#include "ColorObject.h"
#include "MiscFunctions.h"
#include "Render.h"

enum UiElementTypes {
	SLIDER,
	BUTTON,
	TAB,
	CTRLHANDLE,
	PANEL
};

struct UiElement {
	std::string name;
	int id;
	UiElementTypes type;
	bool is_selected;
	virtual std::vector<AbstractBaseObject*> GetObjects() = 0;
	virtual void SetSelected(bool _isSelected) = 0;
	virtual std::vector<UiElement*> GetElements() = 0;
};

struct Handle : UiElement {
	ColorObject obj;

	Handle() {
		type = CTRLHANDLE;
	}

	std::vector<AbstractBaseObject*> GetObjects() {
		std::vector<AbstractBaseObject*> ret;
		ret.push_back(&obj);
		return ret;
	}

	std::vector<UiElement*> GetElements() {
		std::vector<UiElement*> ret;
		ret.push_back(this);
		return ret;
	}

	void SetSelected(bool _isSelected) {
		obj.SetSelected(_isSelected);
	}
};

struct Button : UiElement {

	TextureObject label;
	ColorObject button;
	bool is_pressed;

	Button() {
		is_pressed = false;
		type = BUTTON;
	}

	std::vector<AbstractBaseObject*> GetObjects() {
		std::vector<AbstractBaseObject*> ret;
		ret.push_back(&button);
		ret.push_back(&label);
		return ret;
	}

	std::vector<UiElement*> GetElements() {
		std::vector<UiElement*> ret;
		ret.push_back(this);
		return ret;
	}

	void SetSelected(bool _isSelected) {
		button.SetSelected(_isSelected);
	}
};

struct Slider : UiElement {

	TextureObject name_label;
	TextureObject range_label;
	TextureObject curr_label;

	ColorObject knob;
	ColorObject track;

	float min;
	float max;
	float curr;
	bool has_changed;
	glm::vec2 track_dim;
	glm::vec2 knob_dim;
	glm::vec2 track_padding;
	glm::vec2 knob_padding;
	glm::vec2 track_min_max;
	int element_index;
	float element_y;
	float z_inc;

	Slider() {
		min = 0;
		max = 0;
		curr = 0;
		name = "";
		element_index = 0;
		element_y = 0.0f;
		z_inc = 0.0f;
		type = SLIDER;
		has_changed = false;
	}

	void Set(float _min, float _max, float _curr) {
		min = _min;
		max = _max;
		curr = _curr;
	}

	void UpdateVal(float _x, bool _alertChange) {
		curr = min + _x*(max - min);
		curr_label.GenerateText(MiscFunctions::to_string_with_precision(curr, 4), glm::vec2(knob_dim.x*3.0f, knob_dim.y), glm::vec2(0.0f), glm::vec3(knob_dim.x + knob.model_position.x,element_y * (float)element_index, z_inc*2.0f + 0.0001f));
		if (_alertChange) {
			has_changed = true;
		}
	}

	std::vector<AbstractBaseObject*> GetObjects() {
		std::vector<AbstractBaseObject*> ret;
		ret.push_back(&knob);
		ret.push_back(&track);
		ret.push_back(&name_label);
		ret.push_back(&curr_label);

		return ret;
	}

	std::vector<UiElement*> GetElements() {
		std::vector<UiElement*> ret;
		ret.push_back(this);
		return ret;
	}

	std::string GetLabelString() {
		return name + " [" + MiscFunctions::to_string_with_precision(min, 8) + "," + MiscFunctions::to_string_with_precision(max, 8) + "]";
	}

	void SetSelected(bool _isSelected) {
		knob.SetSelected(_isSelected);
	}
};

struct Tab : UiElement {

	Button activator;
	std::vector<UiElement*> elements;

	float tab_y;
	float tab_x;

	float z_inc;

	float content_y;
	float element_y;

	Tab() {
		type = TAB;
	}

	void AddElement(UiElement* u) {
		elements.push_back(u);
	}

	void Select(bool _isSelected) {
		SetSelected(_isSelected);
		std::vector<AbstractBaseObject*> abObjs = GetObjects();
		for (AbstractBaseObject* abso : abObjs) {
			if(abso != &activator.button && abso != &activator.label)
			abso->is_hidden = !_isSelected;
		}
	}

	void GenerateObjects(glm::vec3 _panelSize, int tab_index, int num_tabs) {

		tab_y = _panelSize.y * 0.2f;
		tab_x = _panelSize.x / (float)num_tabs;
		
		z_inc = _panelSize.z / 3.0f;
		
		content_y = _panelSize.y * 0.8f;
		element_y = content_y / (float)elements.size();

		activator.button.GenerateXYPrism(tab_x, tab_y, z_inc*2.0f, glm::vec2(0.01f, 0.0f), glm::vec3(tab_x*(float)tab_index, content_y, 0.0f));
		activator.label.GenerateText(name,glm::vec2(tab_x, tab_y), glm::vec2(tab_x, tab_y) / 15.0f, glm::vec3(tab_x*(float)tab_index, content_y, z_inc*2.0f + 0.001f));
		activator.button.SetDisplayColor(glm::vec4(0.4f, 0.1f, 1.0f,1.0f));

		for (int i = 0; i < elements.size(); ++i) {

			if (elements[i]->type == SLIDER) {
				Slider * s = static_cast<Slider*>(elements[i]);
				
				// save position vars to update text later
				s->element_index = i;
				s->element_y = element_y;
				s->z_inc = z_inc;
				s->curr = s->min;

				// leave 0.2f of element_y for spacing between elements
				float label_y = 0.3f * element_y;
				float track_y = 0.5f * element_y;

				s->knob_dim = glm::vec2(_panelSize.x / 10.0f, track_y);
				s->track_min_max = glm::vec2(0.0f, _panelSize.x - s->knob_dim.x);

				s->track.GenerateXYPrism(_panelSize.x, track_y, z_inc, glm::vec2(0.0f,s->knob_dim.y / 5.0f), glm::vec3(0.0f, element_y * (float)i, 0.0f));
				s->track.SetDisplayColor(glm::vec4(0.7f, 0.7f, 0.7f,1.0f));

				s->knob.GenerateXYPrism(s->knob_dim.x, s->knob_dim.y, z_inc*3.0f, glm::vec2(0.0f,0.0f), glm::vec3(0.0f, element_y * (float)i, 0.0f));
				s->knob.SetDisplayColor(glm::vec4(1.0f, 0.2f, 0.2f,1.0f));

				s->name_label.GenerateText(s->GetLabelString(), glm::vec2(_panelSize.x, label_y), glm::vec2(0.0f), glm::vec3(0.0f, element_y * (float)i + track_y, 0.0f));
				s->curr_label.GenerateText(MiscFunctions::to_string_with_precision(s->curr, 4), glm::vec2(s->knob_dim.x*3.0f, s->knob_dim.y), glm::vec2(0.0f), glm::vec3(s->knob_dim.x, element_y * (float)i, z_inc*2.0f + 0.0001f));
	
			}
		}
	}

	std::vector<AbstractBaseObject*> GetObjects() {
		std::vector<AbstractBaseObject*> ret;
		ret.push_back(&activator.button);
		ret.push_back(&activator.label);
		for (UiElement* u : elements) {
			std::vector<AbstractBaseObject*> bsos = u->GetObjects();
			ret.insert(ret.end(), bsos.begin(), bsos.end());
		}
		return ret;
	}

	std::vector<UiElement*> GetElements() {
		std::vector<UiElement*> ret;
		ret.push_back(this);
		for (UiElement* u : elements) {
			std::vector<UiElement*> eles = u->GetElements();
			ret.insert(ret.end(), eles.begin(), eles.end());
		}
		return ret;
	}

	void SetSelected(bool _isSelected) {
		activator.SetSelected(_isSelected);
	}

};

struct Panel : UiElement {

	std::vector<Tab*> tabs;
	ColorObject back_plate;
	Handle controller_handle;
	glm::vec3 size;
	glm::vec3 half_size;

	Panel() {
		type = PANEL;
	}

	void AddTab(Tab* t) {
		tabs.push_back(t);
	}

	void SetSize(float _x, float _y, float _z) {
		size = glm::vec3(_x, _y, _z);
	}
	void SetSize(glm::vec3 _size) {
		size = _size;
		half_size = _size * 0.5f;
	}

	void Finalize() {
		back_plate.GenerateXYPrism(size.x, size.y, size.z/2.0f, glm::vec2(-0.05f,-0.05f), glm::vec3(0.0f, 0.0f, (-size.z/2.0f)-0.0001f));
		back_plate.SetDisplayColor(glm::vec4(0.4f, 0.4f, 0.4f,1.0f));
		controller_handle.obj.GenerateSphere(11, size.y / 8.0f, false);
		controller_handle.obj.SetDisplayColor(glm::vec4(1.0f, 0.2f, 0.2f,1.0f));
		controller_handle.obj.Set_model_position(glm::vec3(-0.05f, size.y + size.y / 15.0f, 0.0f));
		for (int i = 0; i < tabs.size(); ++i) {
			tabs[i]->GenerateObjects(size, i, tabs.size());
			tabs[i]->Select(i == 0);
		}
	}

	std::vector<UiElement*> GetElements() {
		std::vector<UiElement*> ret;
		ret.push_back(&controller_handle);
		for (Tab*t : tabs) {
			std::vector<UiElement*> eles = t->GetElements();
			ret.insert(ret.end(), eles.begin(), eles.end());
		}
		return ret;
	}

	std::vector<AbstractBaseObject*> GetObjects() {
		std::vector<AbstractBaseObject*> ret;
		ret.push_back(&back_plate);
		ret.push_back(&controller_handle.obj);
		for (Tab *t: tabs) {
			std::vector<AbstractBaseObject*> bsos = t->GetObjects();
			ret.insert(ret.end(), bsos.begin(), bsos.end());
		}

		return ret;
	}

	void SetSelected(bool _isSelected) {

	}
};

class UiPanel{

	public:
		UiPanel();
		~UiPanel();
		glm::vec3 collision_point;
		glm::mat4 m;
		Panel base_panel;
		std::vector<UiElement*> all_elements;
		void Finalize();
		void GenerateDicomPanel(Render * _r);
		void Interact(glm::mat4 _controllerPose, glm::vec3 _ray, glm::vec3 _pos, bool _pressed, bool world_space);
		std::vector<AbstractBaseObject*> GetObjects();
		void SetWorldPosition(glm::vec3 _inPos);
		void SetModelOrientation(glm::vec3 _inPos);
		void SetAppendPose(glm::mat4 _inPose);
		Slider* GetSliderByName(std::string _name);
		glm::mat4 GetModelMatrix();
		UiElement* GetSelectedElement(glm::mat4 _controllerPose, glm::vec3 _ray, glm::vec3 _pos, bool world_space);
		glm::vec3 GetCollisionPointWithPanelPlane(glm::vec3 _ray, glm::vec3 _pos, bool world_space);
};

