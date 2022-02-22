#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Drawable.h"
#include "Callback.h"

class Button : public Drawable, public Callback<int> {
public:
	enum class Alignment {
		LEFT,
		RIGHT,
		CENTER
	};

	Button(Drawable::Rectangle rect = Drawable::Rectangle(0, 0, 10, 10));
	void Draw(sf::RenderTarget& target) override;

	void Event(sf::Event& event) override;

	void SetText(std::string str);
	void SetGraphic(std::vector<sf::Vertex> graphic);
	void SetFont(const sf::Font& font);
	void SetTextColor(sf::Color color);
	void SetColor(sf::Color color);
	void SetBorderColor(sf::Color color);
	void SetBorderWidth(float num);
	void SetTextAlignment(Alignment alignment);
	void SetID(int id);

	void SetX(float num);
	void SetY(float num);
	void SetWidth(float num) override;
	void SetHeight(float num) override;

	std::string GetText() const;
	std::vector<sf::Vertex> GetGraphic() const;
	unsigned int GetTextSize() const;
	const sf::Font* GetFont() const;
	sf::Color GetTextColor() const;
	sf::Color GetColor() const;
	sf::Color GetBorderColor() const;
	float GetBorderWidth() const;
	Button::Alignment GetTextAlignment() const;
	int GetID() const;

private:
	sf::RectangleShape button;
	sf::Text text;
	std::vector<sf::Vertex> graphic;
	Button::Alignment alignment;
	int id;	
};