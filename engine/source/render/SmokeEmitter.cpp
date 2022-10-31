#include "SmokeEmitter.h"
#include <algorithm>

namespace engine
{
	std::default_random_engine SmokeEmitter::s_random_engine;

	void SmokeEmitter::Update(const std::chrono::steady_clock::time_point& currentTime, float deltaTime)
	{
		float now = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime.time_since_epoch()).count();
		auto it = std::remove_if(m_particles.begin(), m_particles.end(), [&now](Particle& particle)
			{
				return now - particle.creationTime
					> particle.lifeTime;
			});
		m_particles.erase(it, m_particles.end());

		if (m_timer.TimeElapsed(m_spawnPeriod))
		{
			uint32_t numNewInstances = std::max<uint32_t>(deltaTime / m_spawnPeriod, 1);

			for (uint32_t i = 0; i < numNewInstances; ++i)
			{
				m_particles.push_back(Particle(m_position + Vec3(m_random(s_random_engine), m_random(s_random_engine) / 2.f, m_random(s_random_engine)),
					Vec2(m_spawnParticleSize, m_spawnParticleSize),
					m_random(s_random_engine, std::uniform_real<float>::param_type(-M_PI, M_PI)),
					Vec4(m_tint.e[0], m_tint.e[1], m_tint.e[2], 0),
					std::chrono::duration_cast<std::chrono::duration<float>>(currentTime.time_since_epoch()).count(),
					m_particleSaturateTime + m_particleFadingTime));
			}
		}

		float currentLifeDuration;
		for (auto& particle : m_particles)
		{
			particle.position += Vec3(0, m_particleSpeed * deltaTime/FRAME_DURATION, 0);
			particle.size *= m_scalingFactor * deltaTime / FRAME_DURATION;
			currentLifeDuration = now - particle.creationTime;
			if (currentLifeDuration <= m_particleSaturateTime)
			{
				particle.tint.e[3] += deltaTime / m_particleSaturateTime;
				if (particle.tint.e[3] > 1.f) particle.tint.e[3] = 1.f;
			}
			else
			{
				particle.tint.e[3] -= deltaTime / m_particleFadingTime;
				if (particle.tint.e[3] < 0.f) particle.tint.e[3] = 0.f;
			}
			particle.thickness = particle.size.e[0]/1.2f;
		}
	}
}
