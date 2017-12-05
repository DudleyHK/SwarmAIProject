/*



*/


cbuffer ParticleConstantBuffer
{
	float3 goalPosition;
	float3 bestPosition;
	float  basicForce;
	float  gravityAcceleration;
};


struct ParticleStruct
{
	float3 position;
	float3 velocity;
	float3 forces;
	float3 gravity;
	float  speed;
	float  mass;
};

StructuredBuffer<ParticleStruct>   gInput;
RWStructuredBuffer<ParticleStruct> gOutput;





//float CalculateDistance(float3 a, float3 b)
//{
//	float x = (b.x - a.x);
//	float y = (b.y - a.y);
//	float z = (b.z - a.z);
//
//	return (x * x) + (y * y) + (z * z);
//}
//
//
//
//float3 CalculateDirection(float3 a, float3 b)
//{
//	float x = b.x - a.x;
//	float y = b.y - a.y;
//	float z = b.z - a.z;
//
//	return float3(x, y, z);
//}
//
//float3 ComputeForce(float3 dir, float mass)
//{
//	float x = (dir.x * basicForce) * mass;
//	float y = (dir.y * basicForce) * mass;
//	float z = (dir.z * basicForce) * mass;
//
//	return float3(x, y, z);
//}
//
//float3 GetForce(ParticleStruct p)
//{
//	// check if this particle has a closer distance
//	float3 dir           = CalculateDirection(p.position, bestPosition);
//	float3 normalisedDir = 1.0 / length(dir);
//	float3 force         = ComputeForce(dir, p.mass);
//	
//	return p.gravity + force;
//}
//
//
//float3 GetVelocity(ParticleStruct p)
//{
//	float3 acc = p.forces / p.mass;
//	float3 deltaVel = acc * 0.01f;
//	return deltaVel;
//}
//
//
//float3 GetPosition(ParticleStruct p)
//{
//	float3 deltaSpeed = p.velocity * 0.01f;
//	return p.position + deltaSpeed;
//}


[numthreads(32, 16, 1)]
void ParticlePhysicsShader(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	const unsigned int dtid = dispatchThreadID.x;
	ParticleStruct p = gInput[dtid];

	// reset forces to 0
	p.forces.x = bestPosition.x;
	p.forces.y = bestPosition.y;
	p.forces.z = bestPosition.z;


	// TEST THESE VALUES
	//p.forces   =  GetForce(p);
	//p.velocity = GetVelocity(p);
	//p.position = GetPosition(p);
	//p.speed    = length(p.velocity);

	p.speed = 0;




	// append the normalised vector to output buffer.
	//gOutput[dtid].gravity  = p.gravity;
	gOutput[dtid].forces   = p.forces;
	//gOutput[dtid].velocity = p.velocity;
	gOutput[dtid].position = p.position;
	gOutput[dtid].speed    = p.speed;
}