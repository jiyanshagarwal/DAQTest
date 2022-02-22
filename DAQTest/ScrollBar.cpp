#include "ScrollBar.h"

const float ScrollBar::MIN_GRIP_SIZE = 5;

ScrollBar::ScrollBar(float x, float y, float thickness, float length, Orientation orientation) : Drawable(Drawable::Rectangle(x, y, 0, 0)) {
	if (orientation == Orientation::HORIZONTAL) {
		rect.width = length;
		rect.height = thickness;
	}
	else {
		rect.width = thickness;
		rect.height = length;
	}

	this->length = length;
	this->thickness = thickness;
	this->orientation = orientation;
	grip_position = 0;
	bar_size_ratio = 1;

	prev_mouse_x = 0;
	prev_mouse_y = 0;

	SetThickness(thickness);	//This calculates the values of the derived properties

	grip_color = sf::Color(150, 150, 150);
	track_color = sf::Color(230, 230, 230);

	InitializeButtons();
}

ScrollBar& ScrollBar::operator=(const ScrollBar& ref) {
	/*
	* Here's why this is needed:
	* When a scrollbar object is copied, the "this" pointer of the button callback method points to the ref object instead of the current "this".
	* Thus, the onclick function pointers of the buttons must be reset to point to the actually current object "this".
	*/
	Drawable::operator=(ref);

	this->length = ref.length;
	this->thickness = ref.thickness;
	this->orientation = ref.orientation;
	this->grip_position = ref.grip_position;
	this->bar_size_ratio = ref.bar_size_ratio;

	this->trackLength = ref.trackLength;
	this->gripLength = ref.gripLength;
	this->gripThickness = ref.gripThickness;
	this->gripOffset = ref.gripOffset;

	this->prev_mouse_x = ref.prev_mouse_x;
	this->prev_mouse_y = ref.prev_mouse_y;

	this->grip_color = ref.grip_color;
	this->track_color = ref.track_color;

	this->scroll_increase_button = ref.scroll_increase_button;
	this->scroll_decrease_button = ref.scroll_decrease_button;

	scroll_increase_button.OnClick<ScrollBar>(this, &ScrollBar::CalculateScroll);
	scroll_decrease_button.OnClick<ScrollBar>(this, &ScrollBar::CalculateScroll);

	return *this;
}

void ScrollBar::Tick() {
	scroll_increase_button.Tick();
	scroll_decrease_button.Tick();
}

void ScrollBar::Draw(sf::RenderTarget& target) {
	if (bar_size_ratio < 1) {
		scroll_increase_button.Draw(target);
		scroll_decrease_button.Draw(target);

		if (orientation == Orientation::HORIZONTAL) {
			sf::RectangleShape track(sf::Vector2f(trackLength, thickness));
			track.setPosition(rect.x + thickness, rect.y);
			track.setFillColor(track_color);

			sf::RectangleShape grip(sf::Vector2f(gripLength, gripThickness));
			grip.setPosition(rect.x + gripOffset, rect.y + thickness / 4);
			grip.setFillColor(grip_color);

			Drawable::Rectangle increaseButtonBounds = scroll_increase_button.GetBounds();
			Drawable::Rectangle decreaseButtonBounds = scroll_decrease_button.GetBounds();

			sf::Vertex arrows[6] = {
				sf::Vertex(sf::Vector2f(increaseButtonBounds.x + thickness / 3, increaseButtonBounds.y + thickness / 4), grip_color),
				sf::Vertex(sf::Vector2f(increaseButtonBounds.x + thickness / 3, increaseButtonBounds.y + 3 * thickness / 4), grip_color),
				sf::Vertex(sf::Vector2f(increaseButtonBounds.x + 2 * thickness / 3, increaseButtonBounds.y + thickness / 2), grip_color),

				sf::Vertex(sf::Vector2f(decreaseButtonBounds.x + 2 * thickness / 3, decreaseButtonBounds.y + thickness / 4), grip_color),
				sf::Vertex(sf::Vector2f(decreaseButtonBounds.x + 2 * thickness / 3, decreaseButtonBounds.y + 3 * thickness / 4), grip_color),
				sf::Vertex(sf::Vector2f(decreaseButtonBounds.x + thickness / 3, decreaseButtonBounds.y + thickness / 2), grip_color)
			};

			target.draw(track);
			target.draw(grip);
			target.draw(arrows, 6, sf::Triangles);
		}
		else {
			sf::RectangleShape track(sf::Vector2f(thickness, trackLength));
			track.setPosition(rect.x, rect.y + thickness);
			track.setFillColor(track_color);

			sf::RectangleShape grip(sf::Vector2f(gripThickness, gripLength));
			grip.setPosition(rect.x + thickness / 4, rect.y + gripOffset);
			grip.setFillColor(grip_color);

			Drawable::Rectangle increaseButtonBounds = scroll_increase_button.GetBounds();
			Drawable::Rectangle decreaseButtonBounds = scroll_decrease_button.GetBounds();

			sf::Vertex arrows[6] = {
				sf::Vertex(sf::Vector2f(increaseButtonBounds.x + thickness / 4, increaseButtonBounds.y + thickness / 3), grip_color),
				sf::Vertex(sf::Vector2f(increaseButtonBounds.x + 3 * thickness / 4, increaseButtonBounds.y + thickness / 3), grip_color),
				sf::Vertex(sf::Vector2f(increaseButtonBounds.x + thickness / 2, increaseButtonBounds.y + 2 * thickness / 3), grip_color),

				sf::Vertex(sf::Vector2f(decreaseButtonBounds.x + thickness / 4, decreaseButtonBounds.y + 2 * thickness / 3), grip_color),
				sf::Vertex(sf::Vector2f(decreaseButtonBounds.x + 3 * thickness / 4, decreaseButtonBounds.y + 2 * thickness / 3), grip_color),
				sf::Vertex(sf::Vector2f(decreaseButtonBounds.x + thickness / 2, decreaseButtonBounds.y + thickness / 3), grip_color)
			};

			target.draw(track);
			target.draw(grip);
			target.draw(arrows, 6, sf::Triangles);
		}
	}
}

void ScrollBar::Event(sf::Event& event) {
	/*
	* This switch is taken and modifid from the Drawable interface. This way mous_held is based on the grip rather than the entire bar
	*/
	switch (event.type) {
	case sf::Event::MouseButtonPressed:
		if (event.mouseButton.button == sf::Mouse::Left && MouseWithinGrip()) {
			mouse_held = true;
		}
		break;
	case sf::Event::MouseButtonReleased:
		if (event.mouseButton.button == sf::Mouse::Left) {
			mouse_held = false;
		}
		break;
	}

	scroll_increase_button.Event(event);
	scroll_decrease_button.Event(event);

	if (HasFocus()) {
		if (event.type == sf::Event::KeyPressed) {
			CalculateScroll(event.key.code);
		}
		if (mouse_held) {
			if (orientation == Orientation::HORIZONTAL) {
				float delta_x = mouse_x - prev_mouse_x;
				grip_position += delta_x / (trackLength - gripLength);
			}
			else {
				float delta_y = mouse_y - prev_mouse_y;
				grip_position += delta_y / (trackLength - gripLength);
			}

			if (grip_position < 0) {
				grip_position = 0;
			}
			else if (grip_position > 1) {
				grip_position = 1;
			}

			gripOffset = thickness + (trackLength - gripLength) * grip_position;
		}
	}
}

void ScrollBar::SetMousePosition(float x, float y) {
	prev_mouse_x = mouse_x;
	prev_mouse_y = mouse_y;

	Drawable::SetMousePosition(x, y);
	scroll_increase_button.SetMousePosition(x, y);
	scroll_decrease_button.SetMousePosition(x, y);
}

void ScrollBar::ResetGripPosition() {
	grip_position = 0;
}

void ScrollBar::SetBarSizeRatio(float val) {
	bar_size_ratio = val;

	gripLength = trackLength * bar_size_ratio;

	if (gripLength < MIN_GRIP_SIZE) {
		gripLength = MIN_GRIP_SIZE;
	}

	if (bar_size_ratio >= 1) {
		grip_position = 0;
	}

	gripOffset = thickness + (trackLength - gripLength) * grip_position;
}

void ScrollBar::SetLength(float val) {
	length = val;

	if (orientation == Orientation::HORIZONTAL) {
		scroll_increase_button.SetX(rect.x + length - thickness);
		rect.width = length;
	}
	else {
		scroll_increase_button.SetY(rect.y + length - thickness);
		rect.height = length;
	}

	trackLength = length - (2 * thickness);
	gripLength = trackLength * bar_size_ratio;

	if (gripLength < MIN_GRIP_SIZE) {
		gripLength = MIN_GRIP_SIZE;
	}

	gripOffset = thickness + (trackLength - gripLength) * grip_position;
}

void ScrollBar::SetThickness(float val) {
	thickness = val;
	scroll_increase_button.SetWidth(thickness);
	scroll_increase_button.SetHeight(thickness);
	scroll_decrease_button.SetWidth(thickness);
	scroll_decrease_button.SetHeight(thickness);

	if (orientation == Orientation::HORIZONTAL) {
		scroll_increase_button.SetX(rect.x + length - thickness);
		rect.height = thickness;
	}
	else {
		scroll_increase_button.SetY(rect.y + length - thickness);
		rect.width = thickness;
	}

	trackLength = length - (2 * thickness);
	gripLength = trackLength * bar_size_ratio;
	gripThickness = thickness / 2;

	if (gripLength < MIN_GRIP_SIZE) {
		gripLength = MIN_GRIP_SIZE;
	}

	gripOffset = thickness + (trackLength - gripLength) * grip_position;
}

void ScrollBar::SetX(float num) {
	Drawable::SetX(num);
	scroll_increase_button.SetX(num + length - thickness);
	scroll_decrease_button.SetX(num);
}

void ScrollBar::SetY(float num) {
	Drawable::SetY(num);
	scroll_increase_button.SetY(num + length - thickness);
	scroll_decrease_button.SetY(num);
}

void ScrollBar::SetWidth(float num) {
	Drawable::SetWidth(num);

	if (orientation == Orientation::HORIZONTAL) {
		SetLength(rect.width);
	}
	else {
		SetThickness(rect.width);
	}
}

void ScrollBar::SetHeight(float num) {
	Drawable::SetHeight(num);

	if (orientation == Orientation::HORIZONTAL) {
		SetThickness(rect.height);
	}
	else {
		SetLength(rect.height);
	}
}

void ScrollBar::SetFocus(bool val) {
	Drawable::SetFocus(val);
	scroll_increase_button.SetFocus(val);
	scroll_decrease_button.SetFocus(val);
}

void ScrollBar::SetGripColor(sf::Color color) {
	grip_color = color;
}

void ScrollBar::SetTrackColor(sf::Color color) {
	track_color = color;
	scroll_increase_button.SetColor(track_color);
	scroll_decrease_button.SetColor(track_color);
}

float ScrollBar::GetGripPosition() const {
	return grip_position;
}

sf::Color ScrollBar::GetGripColor() const {
	return grip_color;
}

sf::Color ScrollBar::GetTrackColor() const {
	return track_color;
}

bool ScrollBar::isActive() const {
	return bar_size_ratio < 1;
}

void ScrollBar::CalculateScroll(int key) {
	if (bar_size_ratio >= 1) {
		grip_position = 0;
	}
	else {
		if ((key == sf::Keyboard::Left && orientation == Orientation::HORIZONTAL) ||
			(key == sf::Keyboard::Up && orientation == Orientation::VERTICAL)) {
			grip_position = grip_position - 0.05f <= 0 ? 0 : grip_position - 0.05f;
		}
		else if ((key == sf::Keyboard::Right && orientation == Orientation::HORIZONTAL) ||
			(key == sf::Keyboard::Down && orientation == Orientation::VERTICAL)) {
			grip_position = grip_position + 0.05f >= 1 ? 1.0f : grip_position + 0.05f;
		}
	}

	gripOffset = thickness + (trackLength - gripLength) * grip_position;
}

bool ScrollBar::MouseWithinGrip() const {
	if (orientation == Orientation::HORIZONTAL) {
		return ((mouse_x >= rect.x + gripOffset) && (mouse_x <= rect.x + gripOffset + gripLength) &&
			(mouse_y >= rect.y + thickness / 4) && (mouse_y <= rect.y + thickness / 4 + gripThickness));
	}
	else {
		return ((mouse_x >= rect.x + thickness / 4) && (mouse_x <= rect.x + thickness / 4 + gripThickness) &&
			(mouse_y >= rect.y + gripOffset) && (mouse_y <= rect.y + gripOffset + gripLength));
	}
}

void ScrollBar::InitializeButtons() {
	scroll_decrease_button = Button(Drawable::Rectangle(rect.x, rect.y, thickness, thickness));

	if (orientation == Orientation::HORIZONTAL) {
		scroll_increase_button = Button(Drawable::Rectangle(rect.x + length - thickness, rect.y, thickness, thickness));
		scroll_increase_button.SetID(sf::Keyboard::Right);
		scroll_decrease_button.SetID(sf::Keyboard::Left);
	}
	else {
		scroll_increase_button = Button(Drawable::Rectangle(rect.x, rect.y + length - thickness, thickness, thickness));
		scroll_increase_button.SetID(sf::Keyboard::Down);
		scroll_decrease_button.SetID(sf::Keyboard::Up);
	}

	scroll_increase_button.SetBorderWidth(0);
	scroll_increase_button.SetColor(track_color);

	scroll_decrease_button.SetBorderWidth(0);
	scroll_decrease_button.SetColor(track_color);

	scroll_increase_button.OnClick<ScrollBar>(this, &ScrollBar::CalculateScroll);
	scroll_decrease_button.OnClick<ScrollBar>(this, &ScrollBar::CalculateScroll);
}