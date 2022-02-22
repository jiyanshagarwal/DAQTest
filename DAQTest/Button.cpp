#include <SFML/Graphics.hpp>
#include "Button.h"

Button::Button(Drawable::Rectangle rect) : Drawable(rect) {
	button.setSize(sf::Vector2f(rect.width, rect.height));
	button.setPosition(rect.x, rect.y);
	button.setOutlineThickness(3);
	button.setFillColor(sf::Color(200, 200, 200));
	button.setOutlineColor(sf::Color(sf::Color::Black));

	text.setString("button");
	text.setFillColor(sf::Color::Black);
	text.setCharacterSize((int)(0.6 * rect.height));

	alignment = Button::Alignment::CENTER;
	id = 0;
}

void Button::Draw(sf::RenderTarget& target) {
	sf::Color original_color = button.getFillColor();

	if (MouseInBounds(mouse_x, mouse_y)) {
		float tint_factor = 0.4f;
		sf::Color lighter = sf::Color(original_color.r + (int)((255 - original_color.r) * tint_factor),
			original_color.g + (int)((255 - original_color.g) * tint_factor),
			original_color.b + (int)((255 - original_color.b) * tint_factor));

		button.setFillColor(lighter);

		if (has_focus && mouse_held) {
			float shade_factor = 0.4f;
			sf::Color darker = sf::Color((int)(original_color.r * (1 - shade_factor)),
				(int)(original_color.g * (1 - shade_factor)), (int)(original_color.b * (1 - shade_factor)));
			button.setFillColor(darker);
		}
	}

	switch (alignment) {
	case Button::Alignment::LEFT:
		text.setPosition(rect.x + (0.2f * rect.height) - text.getLocalBounds().left,
			rect.y + (rect.height - text.getLocalBounds().height) / 2 - text.getLocalBounds().top);
		break;

	case Button::Alignment::RIGHT:
		text.setPosition(rect.x + rect.width - (0.2f * rect.height), rect.y + (rect.height - text.getLocalBounds().height) / 2 - text.getLocalBounds().top);
		break;

	case Button::Alignment::CENTER:
		text.setPosition(rect.x + (rect.width - text.getLocalBounds().width) / 2 - text.getLocalBounds().left,
			rect.y + (rect.height - text.getLocalBounds().height) / 2 - text.getLocalBounds().top);
		break;
	}

	target.draw(button);
	target.draw(text);

	if (graphic.size() > 0) {
		target.draw(&graphic[0], graphic.size(), sf::Lines);
	}

	button.setFillColor(original_color);
}

void Button::Event(sf::Event& event) {
	Drawable::Event(event);

	if (event.type == sf::Event::MouseButtonReleased) {
		if (event.mouseButton.button == sf::Mouse::Left && GetCallbackFunction() != nullptr && MouseInBounds(mouse_x, mouse_y) && has_focus) {
			GetCallbackFunction()(id);
		}
	}
}

void Button::SetText(std::string str) {
	text.setString(str);
}

void Button::SetGraphic(std::vector<sf::Vertex> graphic) {
	this->graphic = graphic;
}

void Button::SetX(float num) {
	Drawable::SetX(num);
	button.setPosition(rect.x, rect.y);
}

void Button::SetY(float num) {
	Drawable::SetY(num);
	button.setPosition(rect.x, rect.y);
}

void Button::SetWidth(float num) {
	Drawable::SetWidth(num);
	button.setSize(sf::Vector2f(rect.width, rect.height));
}

void Button::SetHeight(float num) {
	Drawable::SetHeight(num);
	button.setSize(sf::Vector2f(rect.width, rect.height));
	text.setCharacterSize((int)(0.6 * rect.height));
}

void Button::SetFont(const sf::Font& font) {
	text.setFont(font);
}

void Button::SetTextColor(sf::Color color) {
	text.setFillColor(color);
}

void Button::SetColor(sf::Color color) {
	button.setFillColor(color);
}

void Button::SetBorderColor(sf::Color color) {
	button.setOutlineColor(color);
}

void Button::SetBorderWidth(float num) {
	button.setOutlineThickness(num);
}

void Button::SetTextAlignment(Alignment alignment) {
	this->alignment = alignment;
}

void Button::SetID(int id) {
	this->id = id;
}

std::string Button::GetText() const {
	return text.getString();
}

std::vector<sf::Vertex> Button::GetGraphic() const {
	return graphic;
}

unsigned int Button::GetTextSize() const {
	return text.getCharacterSize();
}

const sf::Font* Button::GetFont() const {
	return text.getFont();
}

sf::Color Button::GetTextColor() const {
	return text.getFillColor();
}

sf::Color Button::GetColor() const {
	return button.getFillColor();
}

sf::Color Button::GetBorderColor() const {
	return button.getOutlineColor();
}

float Button::GetBorderWidth() const {
	return button.getOutlineThickness();
}

Button::Alignment Button::GetTextAlignment() const {
	return alignment;
}

int Button::GetID() const {
	return id;
}