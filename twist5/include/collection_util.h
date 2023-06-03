#pragma once

/** short cut to check if a key exists in a given map */
template<typename T, typename U>
bool hasKey(const T &aMap, const U &aKey) {
	return aMap.find(aKey) != aMap.end();
}
