#include "UiPanel.h"

UiPanel::UiPanel(){

}

UiPanel::~UiPanel(){

}

UiElement* UiPanel::GetSelectedElement(glm::mat4 _controllerPose, glm::vec3 _ray, glm::vec3 _pos) {

	for (int i = 0; i < all_elements.size(); ++i) {

		if (all_elements[i]->type == CTRLHANDLE) {
			Handle * h = static_cast<Handle*>(all_elements[i]);
			if (h->obj.TestCollision(_ray, _pos, collision_point)) {
				m = glm::inverse(_controllerPose) * h->obj.append_pose;
				return h;
			}
		}

		else if (all_elements[i]->type == SLIDER) {
			Slider* s = static_cast<Slider*>(all_elements[i]);
			if (!s->knob.is_hidden && s->knob.TestCollision(_ray, _pos, collision_point)) {
				return s;
			}
		}

		else if (all_elements[i]->type == TAB) {
			Tab* t = static_cast<Tab*>(all_elements[i]);
			if (!t->activator.button.is_hidden && t->activator.button.TestCollision(_ray, _pos, collision_point)) {
				return t;
			}
		}

	}

	return NULL;
}

glm::mat4 UiPanel::GetModelMatrix() {
	return base_panel.back_plate.GetModelMatrix();
}

glm::vec3 UiPanel::GetCollisionPointWithPanelPlane(glm::vec3 _ray, glm::vec3 _pos) {

	glm::mat4 mm = base_panel.back_plate.GetModelMatrix();
	glm::vec3 cg;
	glm::vec4 tri[3];
	tri[0] = mm * glm::vec4(base_panel.back_plate.positions[0], 1.0f);
	tri[1] = mm * glm::vec4(base_panel.back_plate.positions[1], 1.0f);
	tri[2] = mm * glm::vec4(base_panel.back_plate.positions[2], 1.0f);
	MiscFunctions::ray_triangle_intersection(glm::value_ptr(tri[0]), glm::value_ptr(tri[1]), glm::value_ptr(tri[2]), glm::value_ptr(_pos), glm::value_ptr(_ray), glm::value_ptr(cg));
	return glm::vec3(cg.x * tri[0] + cg.y * tri[1] + cg.z * tri[2]);

}

Slider* UiPanel::GetSliderByName(std::string _name) {
	for (UiElement* uie : all_elements) {
		if (uie->name.compare(_name)==0) {
			Slider * s = static_cast<Slider*>(uie);
			return static_cast<Slider*>(uie);
		}
	}
	return NULL;
}

void UiPanel::Interact(glm::mat4 _controllerPose, glm::vec3 _ray, glm::vec3 _pos, bool _pressed) {

	static UiElement * selected_element = NULL;
	static bool once = false;
	static float x_offset = 0.0f;

	// first press, test for collision
	if (_pressed && selected_element == NULL && !once) {
		glm::vec3 p = GetCollisionPointWithPanelPlane(_ray, _pos);
		selected_element = GetSelectedElement(_controllerPose, _ray, _pos);
		once = true;
		
		// slider found, set x offset 
		if (selected_element != NULL){
			
			if (selected_element->type == SLIDER) {
				selected_element->SetSelected(true);
				Slider* s = static_cast<Slider*>(selected_element);
				x_offset = (glm::inverse(GetModelMatrix())*glm::vec4(p, 1.0f)).x - s->knob.model_position.x;
			}
			else if (selected_element->type == TAB) {
				for (Tab* _tab : base_panel.tabs) {
					_tab->Select(false);
				}
				Tab* t = static_cast<Tab*>(selected_element);
				t->Select(true);
				selected_element = NULL;
			}
			else if (selected_element->type == CTRLHANDLE) {
				selected_element->SetSelected(true);
			}
		}

	}
	// press held down and element was selected
	else if (_pressed && selected_element != NULL) {
		glm::vec3 p = GetCollisionPointWithPanelPlane(_ray,_pos);
		if (glm::dot((p - _pos), _ray) > 0) {
			if (selected_element->type == SLIDER) {
				Slider* s = static_cast<Slider*>(selected_element);
				s->knob.model_position.x = glm::clamp((glm::inverse(GetModelMatrix())*glm::vec4(p, 1.0f)).x - x_offset, s->track_min_max.x, s->track_min_max.y);
				s->UpdateVal(s->knob.model_position.x / s->track_min_max.y, false);
			}
		}

		if (selected_element->type == CTRLHANDLE) {
			Handle* h = static_cast<Handle*>(selected_element);
			this->SetAppendPose(_controllerPose*m);
		}	
		
	}
	// press released with a selected element
	else if (!_pressed && selected_element != NULL) {
		if (selected_element->type == SLIDER) {
			Slider* s = static_cast<Slider*>(selected_element);
			s->UpdateVal(s->knob.model_position.x / s->track_min_max.y, true);
		}

		selected_element->SetSelected(false);
		once = false;
		selected_element = NULL;
	}
	// press released with a click miss
	else if (!_pressed && once) {
		once = false;
	}
	
}

void UiPanel::GenerateDicomPanel() {

	base_panel.SetSize(1.25f, 0.5f, 0.025f);

	// tab 1
	Tab * iso_controls = new Tab;
	iso_controls->name = "iso";

	Slider* iso_tol_slider = new Slider;
	iso_tol_slider->min = 0;
	iso_tol_slider->max = MAX_ISOVALUE_TOLERANCE;
	//iso_tol_slider->UpdateVal(DEFAULT_ISOVALUE_TOLERANCE, false);
	iso_tol_slider->name = "isovalue tolerance";	
	
	Slider* iso_val_slider = new Slider;
	iso_val_slider->min = -2000.0f;
	iso_val_slider->max = 2000.0f;
	//iso_val_slider->UpdateVal(DEFAULT_ISOVALUE, false);
	iso_val_slider->name = "isovalue";

	iso_controls->AddElement(iso_tol_slider);
	iso_controls->AddElement(iso_val_slider);

	base_panel.AddTab(iso_controls);
	
	// tab 2
	Tab * scale_controls = new Tab;
	scale_controls->name = "selection scale";

	Slider* scaler_x = new Slider;
	scaler_x->min = 0;
	scaler_x->max = 1.0f;
	scaler_x->name = "X";

	Slider* scaler_y = new Slider;
	scaler_y->min = 0;
	scaler_y->max = 1.0f;
	scaler_y->name = "Y";

	Slider* scaler_z = new Slider;
	scaler_z->min = 0;
	scaler_z->max = 1.0f;
	scaler_z->name = "Z";
	
	scale_controls->AddElement(scaler_x);
	scale_controls->AddElement(scaler_y);
	scale_controls->AddElement(scaler_z);

	base_panel.AddTab(scale_controls);

	Tab * orthoslice = new Tab;
	orthoslice->name = "orthoslice";

	Slider* window_width = new Slider;
	window_width->min = 0;
	window_width->max = 3000;
	window_width->name = "window width";

	Slider* window_center = new Slider;
	window_center->min = 32000;
	window_center->max = -32000;
	window_center->name = "window center";

	orthoslice->AddElement(window_width);
	orthoslice->AddElement(window_center);	
	
	base_panel.AddTab(orthoslice);

	Tab * misc = new Tab;
	misc->name = "misc";

	Slider* reset_bps = new Slider;
	reset_bps->min = 0;
	reset_bps->max = 1;
	reset_bps->name = "reset selection";
	misc->AddElement(window_width);
	base_panel.AddTab(misc);

	Finalize();
}

void UiPanel::Finalize() {
	all_elements = base_panel.GetElements();
	base_panel.Finalize();
}

std::vector<AbstractBaseObject*> UiPanel::GetObjects() {
	return base_panel.GetObjects();
}

void UiPanel::SetWorldPosition(glm::vec3 _inPos) {
	std::vector<AbstractBaseObject*> objs = GetObjects();
	for (AbstractBaseObject* & bso : objs) {
		bso->Set_world_position(_inPos);
	}
}

void UiPanel::SetModelOrientation(glm::vec3 _inOrientation) {
	std::vector<AbstractBaseObject*> objs = GetObjects();
	for (AbstractBaseObject* & bso : objs) {
		bso->Set_model_orientation( _inOrientation);
	}
}

void UiPanel::SetAppendPose(glm::mat4 _inPose) {
	std::vector<AbstractBaseObject*> objs = GetObjects();
	for (AbstractBaseObject* & bso : objs) {
		bso->Set_append_pose(_inPose);
	}
}
