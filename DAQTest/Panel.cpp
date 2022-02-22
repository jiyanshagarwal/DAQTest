#include <SFML/Graphics.hpp>
#include <map>
#include "Panel.h"

const int Panel::SCROLLBAR_THICKNESS = 20;

Panel::Panel(Drawable::Rectangle rect) : Drawable(rect) {
	x_scrollbar = ScrollBar(rect.x, rect.y + rect.height - SCROLLBAR_THICKNESS, SCROLLBAR_THICKNESS, rect.width - SCROLLBAR_THICKNESS, ScrollBar::Orientation::HORIZONTAL);
	y_scrollbar = ScrollBar(rect.x + rect.width - SCROLLBAR_THICKNESS, rect.y, SCROLLBAR_THICKNESS, rect.height - SCROLLBAR_THICKNESS, ScrollBar::Orientation::VERTICAL);

	right_limit = 0;
	bottom_limit = 0;
	x_scrollable = false;
	y_scrollable = false;
	border_width = 0;

	focused_object = nullptr;
}

Panel::~Panel() {}

void Panel::Draw(sf::RenderTarget& target) {
	sf::RenderTexture renderTexture;
	if (renderTexture.create(rect.width, rect.height)) {
		sf::View view(sf::Vector2f(rect.width / 2 + XOffset(), rect.height / 2 + YOffset()), sf::Vector2f(rect.width, rect.height));
		renderTexture.setView(view);
		renderTexture.clear(color);

		//Must be checked backwards so that layers work correctly
		for (auto iter = objectsToDraw.rbegin(); iter != objectsToDraw.rend(); iter++) {
			if (iter->second != nullptr) {
				iter->second->Draw(renderTexture);
			}
		}

		renderTexture.display();
		sf::Sprite sprite(renderTexture.getTexture());
		sprite.setPosition(sf::Vector2f(rect.x, rect.y));

		sf::RectangleShape border(sf::Vector2f(rect.width, rect.height));
		border.setPosition(sf::Vector2f(rect.x, rect.y));
		border.setOutlineColor(border_color);
		border.setOutlineThickness(border_width);
		border.setFillColor(sf::Color::Transparent);

		target.draw(sprite);
		x_scrollbar.Draw(target);
		y_scrollbar.Draw(target);
		target.draw(border);
	}
}

void Panel::Tick() {
	calculateScroll();

	x_scrollbar.Tick();
	y_scrollbar.Tick();

	if (x_scrollbar.isActive() && y_scrollbar.isActive()) {
		x_scrollbar.SetLength(rect.width - SCROLLBAR_THICKNESS);
		y_scrollbar.SetLength(rect.height - SCROLLBAR_THICKNESS);
	}
	else {
		x_scrollbar.SetLength(rect.width);
		y_scrollbar.SetLength(rect.height);
	}

	for (auto iter = objectsToDraw.begin(); iter != objectsToDraw.end(); iter++) {
		if (iter->second != nullptr) {
			iter->second->Tick();
		}
	}
}

void Panel::SetMousePosition(float x, float y) {
	Drawable::SetMousePosition(x, y);

	x_scrollbar.SetMousePosition(x, y);
	y_scrollbar.SetMousePosition(x, y);

	for (auto iter = objectsToDraw.begin(); iter != objectsToDraw.end(); iter++) {
		if (iter->second != nullptr) {
			iter->second->SetMousePosition(x - rect.x + XOffset(), y - rect.y + YOffset());
		}
	}
}

void Panel::Event(sf::Event& event) {
	if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && HasFocus()) {
		//If the scrollbar is directly clicked, it gets focus. This will prevent any objects in the panel that are behind the bar from getting focus.
		x_scrollbar.SetFocus(x_scrollable && x_scrollbar.MouseInBounds(mouse_x, mouse_y));
		y_scrollbar.SetFocus(y_scrollable && y_scrollbar.MouseInBounds(mouse_x, mouse_y));

		calculateFocus();	//This essentially checks if the panel retains its focus or if a child object was clicked and takes the focus away.
	}

	//If no child object took focus, then the scrollbar will.
	x_scrollbar.SetFocus(focused_object == nullptr && this->has_focus);
	y_scrollbar.SetFocus(focused_object == nullptr && this->has_focus);

	//Passing events to all children.
	if (x_scrollable) {
		x_scrollbar.Event(event);
	}
	if (y_scrollable) {
		y_scrollbar.Event(event);
	}

	for (auto iter = objectsToDraw.begin(); iter != objectsToDraw.end(); iter++) {
		if (iter->second != nullptr) {
			iter->second->Event(event);
		}
	}
}

void Panel::AddObject(Drawable* object, unsigned int layer) {
	objectsToDraw.emplace(layer, object);
}

void Panel::XScroll(bool val) {
	x_scrollable = val;

	if (x_scrollable == false) {
		x_scrollbar.ResetGripPosition();
		y_scrollbar.SetLength(rect.height);
	}
	else {
		if (y_scrollable) {
			x_scrollbar.SetLength(rect.width - SCROLLBAR_THICKNESS);
			y_scrollbar.SetLength(rect.height - SCROLLBAR_THICKNESS);
		}
		else {
			x_scrollbar.SetLength(rect.width);
		}
	}
}

void Panel::YScroll(bool val) {
	y_scrollable = val;

	if (y_scrollable == false) {
		y_scrollbar.ResetGripPosition();
		x_scrollbar.SetLength(rect.width);
	}
	else {
		if (x_scrollable) {
			x_scrollbar.SetLength(rect.width - SCROLLBAR_THICKNESS);
			y_scrollbar.SetLength(rect.height - SCROLLBAR_THICKNESS);
		}
		else {
			y_scrollbar.SetLength(rect.height);
		}
	}
}

void Panel::SetX(float num) {
	Drawable::SetX(num);
	x_scrollbar.SetX(num);
	y_scrollbar.SetX(num + rect.width - SCROLLBAR_THICKNESS);
}

void Panel::SetY(float num) {
	Drawable::SetY(num);
	x_scrollbar.SetY(num + rect.height - SCROLLBAR_THICKNESS);
	y_scrollbar.SetY(num);
}

void Panel::SetWidth(float num) {
	Drawable::SetWidth(num);
	y_scrollbar.SetX(num + rect.width - SCROLLBAR_THICKNESS);
	x_scrollbar.SetLength(rect.width - SCROLLBAR_THICKNESS);
}

void Panel::SetHeight(float num) {
	Drawable::SetHeight(num);
	x_scrollbar.SetY(num + rect.height - SCROLLBAR_THICKNESS);
	y_scrollbar.SetHeight(rect.height - SCROLLBAR_THICKNESS);
}

void Panel::SetFocus(bool val) {
	Drawable::SetFocus(val);

	if (!val && focused_object != nullptr) {
		focused_object->SetFocus(false);
		focused_object = nullptr;
	}
}

void Panel::SetColor(sf::Color color) {
	this->color = color;
}

void Panel::SetBorderColor(sf::Color color) {
	this->border_color = color;
}

void Panel::SetBorderWidth(float num) {
	this->border_width = num;
}

sf::Color Panel::GetColor() const {
	return color;
}

sf::Color Panel::GetBorderColor() const {
	return border_color;
}

float Panel::GetBorderWidth() const {
	return border_width;
}

void Panel::calculateFocus() {
	if (focused_object != nullptr) {
		focused_object->SetFocus(false);
		focused_object = nullptr;
	}

	for (auto iter = objectsToDraw.begin(); iter != objectsToDraw.end(); iter++) {
		if (iter->second != nullptr) {
			if (focused_object == nullptr && !x_scrollbar.HasFocus() && !y_scrollbar.HasFocus() &&
				iter->second->MouseInBounds(mouse_x - rect.x + XOffset(), mouse_y - rect.y + YOffset())) {

				iter->second->SetFocus(true);
				focused_object = iter->second;
				return;
			}
		}
	}
}

void Panel::calculateScroll() {
	if (x_scrollable) {
		right_limit = 0;

		for (auto iter = objectsToDraw.begin(); iter != objectsToDraw.end(); iter++) {
			if (iter->second != nullptr) {
				if (iter->second->GetBounds().x + iter->second->GetBounds().width > right_limit) {
					right_limit = iter->second->GetBounds().x + iter->second->GetBounds().width;
				}
			}
		}

		/*
		* This is added so that there is some empty space at the end. This way the last object is fully visible.
		*/
		right_limit += SCROLLBAR_THICKNESS + 5;

		x_scrollbar.SetBarSizeRatio(rect.width / right_limit);
	}

	if (y_scrollable) {
		bottom_limit = 0;

		for (auto iter = objectsToDraw.begin(); iter != objectsToDraw.end(); iter++) {
			if (iter->second != nullptr) {
				if (iter->second->GetBounds().y + iter->second->GetBounds().height > bottom_limit) {
					bottom_limit = iter->second->GetBounds().y + iter->second->GetBounds().height;
				}
			}
		}

		/*
		* This is added so that there is some empty space at the end. This way the last object is fully visible.
		*/
		bottom_limit += SCROLLBAR_THICKNESS + 5;

		y_scrollbar.SetBarSizeRatio(rect.height / bottom_limit);
	}
}

float Panel::XOffset() const {
	return x_scrollbar.GetGripPosition() * (right_limit - rect.width);
}

float Panel::YOffset() const {
	return y_scrollbar.GetGripPosition() * (bottom_limit - rect.height);
}