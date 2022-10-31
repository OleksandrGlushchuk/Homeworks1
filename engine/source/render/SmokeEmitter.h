#pragma once
#include "Particle.h"
#include "../math/Timer.h"
#include <vector>
#include <random>
#define _USE_MATH_DEFINES
#include <math.h>
static const float FRAME_DURATION = 1.f / 60.f;

namespace engine
{
	class SmokeEmitter
	{
		std::vector<Particle> m_particles;
		Vec3 m_position;
		Vec3 m_tint;
		float m_particleSaturateTime;
		float m_particleFadingTime;
		float m_spawnPeriod;
		float m_spawnParticleSize;
		float m_emitterRadius;
		float m_particleSpeed;
		float m_scalingFactor;
		float m_frameDuration;
		uint32_t m_atlasRowsNum;
		uint32_t m_atlasColsNum;
		Timer m_timer;
		static std::default_random_engine s_random_engine;
		std::uniform_real_distribution<float> m_random;
	public:
		SmokeEmitter(){}
		SmokeEmitter(const Vec3& position, const Vec3& tint, float particleSaturateTime, float particleFadingTime, float particleSpeed,
			float spawnPeriod, float spawnParticleSize, float scalingFactor, float emitterRadius, uint32_t atlasRowsNum, uint32_t atlasColsNum):
			m_position(position), m_tint(tint), m_particleSaturateTime(particleSaturateTime), m_particleFadingTime(particleFadingTime),
			m_particleSpeed(particleSpeed),	m_emitterRadius(emitterRadius),	m_spawnPeriod(spawnPeriod), m_spawnParticleSize(spawnParticleSize),
			m_scalingFactor(scalingFactor), m_random(std::uniform_real_distribution<float>(-emitterRadius, emitterRadius)), 
			m_atlasColsNum(atlasColsNum), m_atlasRowsNum(atlasRowsNum)
		{
			m_frameDuration = (m_particleSaturateTime + m_particleFadingTime) / float(m_atlasRowsNum * m_atlasColsNum);
			m_timer.StartTimer();
		}
		void Update(const std::chrono::steady_clock::time_point& currentTime, float deltaTime);
		uint32_t GetParticleNum() { return m_particles.size(); }

		friend class ParticleSystem;
	};
}