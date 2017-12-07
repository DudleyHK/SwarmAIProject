/*



*/


cbuffer ParticleConstantBuffer
{
	float4 goalPosition;
	float4 bestPosition;
	float  basicForce;
	float  gravityAcceleration;
};


struct ParticleStruct
{
	float4 position;
	float4 velocity;
	float4 forces;
	float4 gravity;
	float  speed;
	float  mass;
};

StructuredBuffer<ParticleStruct>   gInput;
RWStructuredBuffer<ParticleStruct> gOutput;





float CalculateDistance(float4 a, float4 b)
{
	float x = (b.x - a.x);
	float y = (b.y - a.y);
	float z = (b.z - a.z);
	float w = 0;

	return (x * x) + (y * y) + (z * z) + (w * w);
}



float4 CalculateDirection(float4 a, float4 b)
{
	float x = b.x - a.x;
	float y = b.y - a.y;
	float z = b.z - a.z;

	return float4(x, y, z, 0);
}

float4 ComputeForce(float4 dir, float mass)
{
	float x = (dir.x * basicForce) * mass;
	float y = (dir.y * basicForce) * mass;
	float z = (dir.z * basicForce) * mass;

	return float4(x, y, z, 0);
}

float4 GetForce(float4 position, float4 gravity, float mass)
{
	// check if this particle has a closer distance
	float4 dir           = CalculateDirection(position, bestPosition);
	float4 normalisedDir = normalize(dir);
	float4 force         = ComputeForce(normalisedDir, mass);
	
	return gravity + force;
}


float4 GetVelocity(float4 forces, float mass)
{
	float4 acc = forces / mass;
	float4 deltaVel = acc * 0.01f;
	return deltaVel;
}


float4 GetPosition(float4 velocity, float4 position)
{
	float4 deltaSpeed = velocity * 0.01f;
	return position + deltaSpeed;
}


[numthreads(32, 16, 1)]
void ParticlePhysicsShader(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	const unsigned int dtid = dispatchThreadID.x;
	ParticleStruct p = gInput[dtid];

	// reset
	p.forces.xyzw  = 0;
	p.gravity.xyzw = 0;

	// set gravity.
	p.gravity.y = p.mass * gravityAcceleration;

	// calculate physics.
	p.forces   = GetForce(p.position, p.gravity, p.mass);
	p.velocity = GetVelocity(p.forces, p.mass);
	p.position = GetPosition(p.velocity, p.position);
	p.speed    = length(p.velocity);

	// update output data.
	gOutput[dtid].position = p.position;
	gOutput[dtid].velocity = p.velocity;
	gOutput[dtid].forces   = p.forces;
	gOutput[dtid].gravity  = p.gravity;
	gOutput[dtid].speed    = p.speed;
}