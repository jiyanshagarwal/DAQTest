#pragma once
#include <functional>

template <typename T>
class Callback {
public:
	Callback() : onClick(nullptr) {}
	void OnClick(std::function<void(T)> func);

	template <typename U>
	void OnClick(U* object, std::function<void(U*, T)> func);

	template <typename V>
	void OnClick(const V* object, std::function<void(const V*, T)> func);

	std::function<void(T)> GetCallbackFunction() const;

private:
	std::function<void(T)> onClick;
};

template <typename T>
void Callback<T>::OnClick(std::function<void(T)> func) {
	onClick = func;
}

template <typename T>
template <typename U>
void Callback<T>::OnClick(U* object, std::function<void(U*, T)> func) {
	OnClick(std::bind(func, object, std::placeholders::_1));
}

template <typename T>
template <typename V>
void Callback<T>::OnClick(const V* object, std::function<void(const V*, T)> func) {
	OnClick(std::bind(func, object, std::placeholders::_1));
}

template <typename T>
std::function<void(T)> Callback<T>::GetCallbackFunction() const {
	return onClick;
}