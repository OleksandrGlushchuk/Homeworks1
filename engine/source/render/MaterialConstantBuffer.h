#pragma once
namespace engine
{
	struct MaterialConstantBuffer
	{
		float metalnessValue;
		float roughnessValue;
		int hasMetalnessMap;
		int hasRoughnessMap;
		int hasNormalMap;
		int reversedNormalTextureY;
		float padding[2];
		MaterialConstantBuffer() {}
		MaterialConstantBuffer(int _hasMetalnessMap, int _hasRoughnessMap, int _hasNormalMap, int _reversedNormalTextureY, float _metalnessValue = {}, float _roughnessValue = {}) :
			hasMetalnessMap(_hasMetalnessMap), hasRoughnessMap(_hasRoughnessMap), hasNormalMap(_hasNormalMap), reversedNormalTextureY(_reversedNormalTextureY),
			metalnessValue(_metalnessValue), roughnessValue(_roughnessValue)
		{}
	};
}