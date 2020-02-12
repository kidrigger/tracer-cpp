#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include <vector>
#include <mutex>
#include <algorithm>

class single_thread_policy {
public:
	void lock(){};
	void unlock(){};
};

class multi_thread_policy {
	std::mutex locke;

public:
	void lock() { locke.lock(); }
	void unlock() { locke.unlock(); }
};

template <class thread_policy>
class framebuffer : public thread_policy {
private:
	int width;
	int height;
	std::vector<vec3> data_vector;

public:
	framebuffer(int width, int height) :
			width(width),
			height(height),
			data_vector(width * height) {}

	size_t size() const { return data_vector.size(); }
	auto begin() { return data_vector.begin(); }
	auto end() { return data_vector.end(); }
	auto data() { return data_vector.data(); }

	vec3 &operator[](int i) { return data_vector[i]; }
	vec3 &operator()(int i, int j) { return data_vector[i + width * j]; }
	template <typename TP>
	[[nodiscard]] std::vector<vec3> operator+(framebuffer<TP> &other) {
		this->lock();
		std::vector<vec3> res(other.size());
		std::transform(data_vector.begin(), data_vector.end(), other.begin(),
				res.begin(), std::plus<vec3>());
		this->unlock();
		return res;
	}
	template <typename TP>
	std::vector<vec3> &operator+=(framebuffer<TP> &other) {
		this->lock();
		std::transform(data_vector.begin(), data_vector.end(), other.begin(),
				data_vector.begin(), std::plus<vec3>());
		this->unlock();
		return data_vector;
	}
};

template <typename TP>
void operator/=(framebuffer<TP> &buffer, float scalar) {
	for (auto &element : buffer) {
		element /= scalar;
	}
}

#endif /* _FRAMEBUFFER_H */
