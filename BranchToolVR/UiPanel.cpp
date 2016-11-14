#include "UiPanel.h"

UiPanel::UiPanel(){

}

UiPanel::~UiPanel(){

}

UiElement* UiPanel::GetSelectedElement(glm::mat4 _controllerPose, glm::vec3 _ray, glm::vec3 _pos, bool world_space) {

	//for (int i = 0; i < all_elements.size(); ++i) 
	//{
	//	if (world_space && all_elements[i]->type == CTRLHANDLE) {
	//		Handle * h = static_cast<Handle*>(all_elements[i]);
	//		if (h->obj.TestCollision(_ray, _pos, collision_point, world_space)) {
	//			m = glm::inverse(_controllerPose) * h->obj.append_pose;
	//			return h;
	//		}
	//	}
	//	else if (all_elements[i]->type == SLIDER) {
	//		Slider* s = static_cast<Slider*>(all_elements[i]);
	//		if (!s->knob.is_hidden && s->knob.TestCollision(_ray, _pos, collision_point, world_space)) {
	//			return s;
	//		}
	//	}
	//	else if (all_elements[i]->type == TAB) {
	//		Tab* t = static_cast<Tab*>(all_elements[i]);
	//		if (!t->activator.button.is_hidden && t->activator.button.TestCollision(_ray, _pos, collision_point, world_space)) {
	//			return t;
	//		}
	//	}
	//}

	return NULL;
}

glm::mat4 UiPanel::GetModelMatrix() {
	return base_panel.back_plate.GetModelMatrix();
}

glm::vec3 UiPanel::GetCollisionPointWithPanelPlane(glm::vec3 _ray, glm::vec3 _pos, bool world_space) {

	glm::mat4 mm;
	
	if (world_space) {
		mm = base_panel.back_plate.GetModelMatrix();
	}
	else {
		mm = base_panel.back_plate.ui_transform;
	}
	
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
		if (uie->name.compare(_name) == 0) {
			Slider * s = static_cast<Slider*>(uie);
			return static_cast<Slider*>(uie);
		}
	}
	return NULL;
}

void UiPanel::Interact(glm::mat4 _controllerPose, glm::vec3 _ray, glm::vec3 _pos, bool _pressed, bool world_space) {

	static UiElement * selected_element = NULL;
	static bool once = false;
	static float x_offset = 0.0f;
	static bool lock = false;

	// first press, test for collision
	if (_pressed && selected_element == NULL && !once) {
		glm::vec3 p = GetCollisionPointWithPanelPlane(_ray, _pos, world_space);
		selected_element = GetSelectedElement(_controllerPose, _ray, _pos, world_space);
		once = true;
		
		if (selected_element != NULL){

			// disallow controller/2d ui interference
			lock = world_space;

			// slider found, set x offset 
			if (selected_element->type == SLIDER) {
				selected_element->SetSelected(true);
				Slider* s = static_cast<Slider*>(selected_element);
				if (world_space) {
					x_offset = (glm::inverse(GetModelMatrix())*glm::vec4(p, 1.0f)).x - s->knob.model_position.x;
				}
				else {
					x_offset = (glm::inverse(base_panel.back_plate.ui_transform)*glm::vec4(p, 1.0f)).x - s->knob.model_position.x;
				}
				
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
	else if (_pressed && selected_element != NULL && lock == world_space) {
		glm::vec3 p = GetCollisionPointWithPanelPlane(_ray,_pos, world_space);
		if (glm::dot((p - _pos), _ray) > 0) {
			if (selected_element->type == SLIDER) {
				Slider* s = static_cast<Slider*>(selected_element);
				if (world_space) {
					s->knob.Set_model_positionX(glm::clamp((glm::inverse(GetModelMatrix())*glm::vec4(p, 1.0f)).x - x_offset, s->track_min_max.x, s->track_min_max.y));
				}
				else {
					s->knob.Set_model_positionX(glm::clamp((glm::inverse(base_panel.back_plate.ui_transform)*glm::vec4(p, 1.0f)).x - x_offset, s->track_min_max.x, s->track_min_max.y));
				}

				s->UpdateVal(s->knob.model_position.x / s->track_min_max.y, false);
			}
		}

		if (selected_element->type == CTRLHANDLE) {
			Handle* h = static_cast<Handle*>(selected_element);
			this->SetAppendPose(_controllerPose*m);
		}	
		
	}
	// press released with a selected element
	else if (!_pressed && selected_element != NULL && lock == world_space) {
		if (selected_element->type == SLIDER) {
			Slider* s = static_cast<Slider*>(selected_element);
			s->UpdateVal(s->knob.model_position.x / s->track_min_max.y, true);
		}

		selected_element->SetSelected(false);
		once = false;
		selected_element = NULL;
	}
	// press released with a click miss
	else if (!_pressed && once && lock == world_space) {
		once = false;
	}
	
}

void UiPanel::GenerateDicomPanel(Render * _r) {

	base_panel.SetSize(Constants::DICOM_PANEL_DIMENSIONS);
	//glm::vec3 half_panel_size = base_panel.size * 0.525f;

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
	scale_controls->name = "scale";

	Slider* scaler_x = new Slider;
	scaler_x->min = 0;
	scaler_x->max = 1.0f;
	scaler_x->name = "selector scale";
	
	scale_controls->AddElement(scaler_x);

	base_panel.AddTab(scale_controls);

	// tab 3
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

	Slider* ortho_level = new Slider;
	ortho_level->min = 0;
	ortho_level->max = 310;
	ortho_level->name = "level";

	orthoslice->AddElement(window_width);
	orthoslice->AddElement(window_center);	
	orthoslice->AddElement(ortho_level);
	
	base_panel.AddTab(orthoslice);

	// tab 4
	Tab * misc = new Tab;
	misc->name = "misc";

	Slider* reset_bps = new Slider;
	reset_bps->min = 0;
	reset_bps->max = 1;
	reset_bps->name = "reset selection";
	misc->AddElement(reset_bps);
	base_panel.AddTab(misc);

	Finalize();

	std::vector<AbstractBaseObject*> objects = base_panel.GetObjects();
	
	// add objects to world scene
	_r->AddObjectToScene(objects);

	// add objects to 2D ui
	for (int i = 0; i < objects.size(); ++i) {
		if (objects[i]->Type() == 0) {
			ColorObject* co = static_cast<ColorObject*>(objects[i]);
			if (co != &base_panel.controller_handle.obj) {
				co->ui_transform = glm::translate(glm::mat4(1.0f), -0.5f*base_panel.size);// *co->base_model_matrix;
				_r->color_ui_elements.push_back(co);
			}
		}
		else if (objects[i]->Type() == 1) {
			TextureObject* to = static_cast<TextureObject*>(objects[i]);
			to->ui_transform = glm::translate(glm::mat4(1.0f), -0.5f*base_panel.size);// *to->base_model_matrix;
			_r->texture_ui_elements.push_back(to);
		}
	}

	_r->ui_quadrant_ortho[0] = glm::vec4(base_panel.half_size.x, base_panel.half_size.y, 0.0f,0.0f);
	_r->ui_quadrant_ortho[1] = glm::vec4(0.5f,0.5f,0.0f,0.0f);
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
