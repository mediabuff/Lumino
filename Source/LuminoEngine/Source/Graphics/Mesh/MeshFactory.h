/*
	"RegularXXXX" �Ƃ������O�̂��Ă���t�@�N�g���́AUV �W�J�����Ƃ��Ɏ��R�ɂȂ�悤�ȃW�I���g���𐶐�����B
	�Ⴆ�� Box ���ƁA�e�N�X�`�����g�p���Ȃ��P�F�� Box �ł���Β��_�� 8 �ł悢�B�e6�ʂ́A���_�����L�ł���B
	�������A�_�C�X�̂悤�Ɋe�ʂɕʂ� UV ��ݒ肵�����ꍇ�͒��_�����L���邱�Ƃ͂ł��Ȃ��B���̏ꍇ�� 24 �̒��_���K�v�ɂȂ�B
	���̂悤�ȃW�I���g�����쐬����̂� "RegularXXXX"�B
*/
#include "../Internal.h"
#include <Lumino/Graphics/Vertex.h>

LN_NAMESPACE_BEGIN
namespace detail {

class MeshHelper
{
public:

	static Vector3 GetXZCirclePoint(int i, int slices)
	{
		float angle = i * Math::PI2 / slices;
		float dx, dz;
		Math::SinCos(angle, &dx, &dz);
		return Vector3(dx, 0, dz);
	}

	static Vector3 GetXZArcPoint(float startAngle, float endAngle, int i, int slices)
	{
		float da = endAngle - startAngle;
		float angle = (i * da / slices) + startAngle;
		float dx, dz;
		Math::SinCos(angle, &dx, &dz);
		return Vector3(dx, 0, dz);
	}

	static void Transform(Vertex* begin, Vertex* end, const Matrix& transform)
	{
		for (Vertex* v = begin; v < end; v++)
		{
			v->position.TransformCoord(transform);
		}
	}
};

class MeshFactoryBase
{
public:
	MeshFactoryBase()
		: m_color(Color::White)
		, m_transform()
	{
	}

	void Initialize(const Color& color, const Matrix& transform)
	{
		m_color = color;
		m_transform = transform;
	}

	Color	m_color;
	Matrix	m_transform;
};

class PlaneMeshFactory
{
public:
	PlaneMeshFactory(const Vector2& size)
	{
		m_size = size;
	}

	int GetVertexCount() const
	{
		return 4;
	}

	int GetIndexCount() const
	{
		return 6;
	}

	void Generate(Vertex* outVertices, uint16_t* outIndices)
	{
		Vector2 half = m_size / 2;
		outVertices[0].position.Set(-half.x, half.y, 0);
		outVertices[0].normal.Set(0.0f, 0.0f, -1.0f);
		outVertices[0].uv.Set(0.0f, 0.0f);
		outVertices[1].position.Set(-half.x, -half.y, 0);
		outVertices[1].normal.Set(0.0f, 0.0f, -1.0f);
		outVertices[1].uv.Set(0.0f, 1.0f);
		outVertices[2].position.Set(half.x, half.y, 0);
		outVertices[2].normal.Set(0.0f, 0.0f, -1.0f);
		outVertices[2].uv.Set(1.0f, 0.0f);
		outVertices[3].position.Set(half.x, -half.y, 0);
		outVertices[3].normal.Set(0.0f, 0.0f, -1.0f);
		outVertices[3].uv.Set(1.0f, 1.0f);

		outIndices[0] = 0;
		outIndices[1] = 1;
		outIndices[2] = 2;
		outIndices[3] = 2;
		outIndices[4] = 1;
		outIndices[5] = 3;
	}

private:
	Vector2	m_size;
};

class PlaneMeshFactory2
{
public:
	PlaneMeshFactory2(const Vector2& size, const Vector3& front)
		: m_size(size)
		, m_front(front)
	{}

	int GetVertexCount() const { return 4; }
	int GetIndexCount() const { return 6; }

	void Generate(Vertex* outVertices, uint16_t* outIndices)
	{
		Vector2 half = m_size / 2;
		if (m_front == Vector3::UnitY)
		{
			outVertices[0].position.Set(-half.x, 0, half.y);
			outVertices[1].position.Set(-half.x, 0, -half.y);
			outVertices[2].position.Set(half.x, 0, half.y);
			outVertices[3].position.Set(half.x, 0, -half.y);
		}
		else if(m_front == -Vector3::UnitY)
		{
			outVertices[0].position.Set(half.x, 0, half.y);
			outVertices[1].position.Set(half.x, 0, -half.y);
			outVertices[2].position.Set(-half.x, 0, half.y);
			outVertices[3].position.Set(-half.x, 0, -half.y);
		}
		else
		{
			LN_NOTIMPLEMENTED();
		}

		outVertices[0].normal = m_front;
		outVertices[0].uv.Set(0.0f, 0.0f);
		outVertices[1].normal = m_front;
		outVertices[1].uv.Set(0.0f, 1.0f);
		outVertices[2].normal = m_front;
		outVertices[2].uv.Set(1.0f, 0.0f);
		outVertices[3].normal = m_front;
		outVertices[3].uv.Set(1.0f, 1.0f);

		outIndices[0] = 0;
		outIndices[1] = 1;
		outIndices[2] = 2;
		outIndices[3] = 2;
		outIndices[4] = 1;
		outIndices[5] = 3;
	}

private:
	Vector2	m_size;
	Vector3	m_front;
};

// xz ���ʁBy+ �������B
class PlaneMeshFactory3
	: public MeshFactoryBase
{
public:
	PlaneMeshFactory3()
		: m_size()
		, m_sliceX(0)
		, m_sliceZ(0)
	{
	}

	void Initialize(const Vector2& size, int sliceX, int sliceZ, const Color& color, const Matrix& transform)
	{
		LN_FAIL_CHECK_ARG(sliceX >= 1) return;
		LN_FAIL_CHECK_ARG(sliceZ >= 1) return;
		m_size = size;
		m_sliceX = sliceX;
		m_sliceZ = sliceZ;
		MeshFactoryBase::Initialize(color, transform);
	}

	int GetVertexCount() const { return (m_sliceX + 1) * (m_sliceZ + 1); }
	int GetIndexCount() const { return (m_sliceX * m_sliceZ * 2) * 3; }

	void Generate(Vertex* outVertices, uint16_t* outIndices, uint16_t beginIndex)
	{
		Vector2 minPos = -m_size / 2;
		Vector2 maxPos = m_size / 2;

		float minX = -m_size.x / 2;
		float maxX = m_size.x / 2;
		float minZ = -m_size.y / 2;
		float maxZ = m_size.y / 2;
		float stepX = (maxPos.x - minPos.x) / m_sliceX;
		float stepZ = (maxPos.y - minPos.y) / m_sliceZ;
		float StepU = 1.0f / m_sliceX;
		float StepV = 1.0f / m_sliceZ;
		Vertex* v = outVertices;
		uint16_t* i = (uint16_t*)outIndices;

		// vertices
		for (int iZ = 0; iZ < m_sliceZ + 1; ++iZ)
		{
			for (int iX = 0; iX < m_sliceX + 1; ++iX)
			{
				if (iX == m_sliceX)
					v->position.x = maxX;	// �덷���o�������Ȃ����ߒ��ڐݒ�
				else
					v->position.x = minX + stepX * iX;

				if (iZ == m_sliceZ)
					v->position.z = minZ;	// �덷���o�������Ȃ����ߒ��ڐݒ�
				else
					v->position.z = maxZ - stepZ * iZ;

				v->position.y = 0.0f;
				v->normal.Set(0.0f, 1.0f, 0.0f);
				v->uv.Set(StepU * iX, 1.0f - StepV * iZ);
				v->color = m_color;
				++v;
			}
		}

		// indices
		for (int iZ = 0; iZ < m_sliceZ; ++iZ)
		{
			for (int iX = 0; iX < m_sliceX; ++iX)
			{
				int p1 = (iX + 0) + (iZ + 0) * (m_sliceX + 1);	// ��
				int p2 = (iX + 0) + (iZ + 1) * (m_sliceX + 1);	// ��
				int p3 = (iX + 1) + (iZ + 0) * (m_sliceX + 1);	// ��
				int p4 = (iX + 1) + (iZ + 1) * (m_sliceX + 1);	// ��
				i[0] = beginIndex + p1;
				i[1] = beginIndex + p2;
				i[2] = beginIndex + p3;
				i[3] = beginIndex + p3;
				i[4] = beginIndex + p2;
				i[5] = beginIndex + p4;
				i += 6;
			}
		}

		if (!m_transform.IsIdentity())
			MeshHelper::Transform(outVertices, v, m_transform);
	}

private:
	Vector2	m_size;
	int		m_sliceX;
	int		m_sliceZ;
};

class BoxMeshFactory
{
public:
	BoxMeshFactory(const Vector3& size)
	{
		m_size = size;
	}

	int GetVertexCount() const
	{
		return 8;
	}

	int GetIndexCount() const
	{
		return 36;
	}

	void Generate(Vertex* outVertices, uint16_t* outIndices)
	{
		Vector3 minPos = -(m_size / 2);
		Vector3 maxPos = (m_size / 2);

		// ��O (Z-)
		outVertices[0].position.Set(minPos.x, maxPos.y, minPos.z);	// ����
		outVertices[0].uv.Set(0.0f, 0.0f);
		outVertices[1].position.Set(minPos.x, minPos.y, minPos.z);	// ����
		outVertices[1].uv.Set(0.0f, 1.0f);
		outVertices[2].position.Set(maxPos.x, maxPos.y, minPos.z);	// �E��
		outVertices[2].uv.Set(1.0f, 0.0f);
		outVertices[3].position.Set(maxPos.x, minPos.y, minPos.z);	// �E��
		outVertices[3].uv.Set(1.0f, 1.0f);
		// �� (Z+)
		outVertices[4].position.Set(minPos.x, maxPos.y, maxPos.z);	// ����
		outVertices[4].uv.Set(1.0f, 0.0f);
		outVertices[5].position.Set(minPos.x, minPos.y, maxPos.z);	// ����
		outVertices[5].uv.Set(1.0f, 1.0f);
		outVertices[6].position.Set(maxPos.x, maxPos.y, maxPos.z);	// �E��
		outVertices[6].uv.Set(0.0f, 0.0f);
		outVertices[7].position.Set(maxPos.x, minPos.y, maxPos.z);	// �E��
		outVertices[7].uv.Set(0.0f, 1.0f);

		for (int i = 0; i < 8; ++i) outVertices[i].color = Color::White;

		uint16_t indices[] =
		{
			// near
			0, 1, 2,
			2, 1, 3,
			// far
			6, 7, 4,
			4, 7, 5,
			// left
			4, 5, 0,
			0, 5, 1,
			// right
			2, 3, 6,
			6, 3, 7,
			// top
			4, 0, 6,
			6, 0, 2,
			// bottom (Z- ���ʂƂ��ď����)
			1, 5, 3,
			3, 5, 7,
		};
		memcpy(outIndices, indices, sizeof(indices));
	}

private:
	Vector3	m_size;
};


// 6�ʂ��ꂼ��Ɨ��������_����������
class RegularBoxMeshFactory
	: public MeshFactoryBase
{
public:
	RegularBoxMeshFactory() {}

	void Initialize(const Vector3& size, const Color& color, const Matrix& transform)
	{
		m_size = size;
		MeshFactoryBase::Initialize(color, transform);
	}

	int GetVertexCount() const
	{
		return 24;
	}

	int GetIndexCount() const
	{
		return 36;
	}

	void SetV(Vertex* vertex, float x, float y, float z, float u, float v, const Vector3& normal)
	{
		vertex->position.Set(x, y, z);
		vertex->uv.Set(u, v);
		vertex->color = m_color;
		vertex->normal = normal;
	}
	static void SetI(uint16_t* index, uint16_t begin)
	{
		index[0] = begin + 0;
		index[1] = begin + 1;
		index[2] = begin + 2;
		index[3] = begin + 2;
		index[4] = begin + 1;
		index[5] = begin + 3;
	}

	void Generate(Vertex* outVertices, uint16_t* outIndices, uint16_t beginVertexIndex)
	{
		Vector3 minPos = -(m_size / 2);
		Vector3 maxPos = (m_size / 2);
		Vertex* v = outVertices;
		uint16_t* i = outIndices;

		// ��O (Z-)
		SetV(v, minPos.x, maxPos.y, minPos.z, 0.0f, 0.0f, -Vector3::UnitZ); ++v;	// ��
		SetV(v, minPos.x, minPos.y, minPos.z, 0.0f, 1.0f, -Vector3::UnitZ); ++v;	// ��
		SetV(v, maxPos.x, maxPos.y, minPos.z, 1.0f, 0.0f, -Vector3::UnitZ); ++v;	// ��
		SetV(v, maxPos.x, minPos.y, minPos.z, 1.0f, 1.0f, -Vector3::UnitZ); ++v;	// ��
		SetI(i, beginVertexIndex + 0); i += 6;

		// �� (Z+)
		SetV(v, maxPos.x, maxPos.y, maxPos.z, 0.0f, 0.0f, Vector3::UnitZ); ++v;	// ��
		SetV(v, maxPos.x, minPos.y, maxPos.z, 0.0f, 1.0f, Vector3::UnitZ); ++v;	// ��
		SetV(v, minPos.x, maxPos.y, maxPos.z, 1.0f, 0.0f, Vector3::UnitZ); ++v;	// ��
		SetV(v, minPos.x, minPos.y, maxPos.z, 1.0f, 1.0f, Vector3::UnitZ); ++v;	// ��
		SetI(i, beginVertexIndex + 4); i += 6;

		// �� (X-)
		SetV(v, minPos.x, maxPos.y, maxPos.z, 0.0f, 0.0f, -Vector3::UnitX); ++v;	// ��
		SetV(v, minPos.x, minPos.y, maxPos.z, 0.0f, 1.0f, -Vector3::UnitX); ++v;	// ��
		SetV(v, minPos.x, maxPos.y, minPos.z, 1.0f, 0.0f, -Vector3::UnitX); ++v;	// ��
		SetV(v, minPos.x, minPos.y, minPos.z, 1.0f, 1.0f, -Vector3::UnitX); ++v;	// ��
		SetI(i, beginVertexIndex + 8); i += 6;

		// �E (X+)
		SetV(v, maxPos.x, maxPos.y, minPos.z, 0.0f, 0.0f, Vector3::UnitX); ++v;	// ��
		SetV(v, maxPos.x, minPos.y, minPos.z, 0.0f, 1.0f, Vector3::UnitX); ++v;	// ��
		SetV(v, maxPos.x, maxPos.y, maxPos.z, 1.0f, 0.0f, Vector3::UnitX); ++v;	// ��
		SetV(v, maxPos.x, minPos.y, maxPos.z, 1.0f, 1.0f, Vector3::UnitX); ++v;	// ��
		SetI(i, beginVertexIndex + 12); i += 6;

		// �� (Y-)(Z- ��UV�̏����)
		SetV(v, minPos.x, minPos.y, minPos.z, 0.0f, 0.0f, -Vector3::UnitY); ++v;	// ��
		SetV(v, minPos.x, minPos.y, maxPos.z, 0.0f, 1.0f, -Vector3::UnitY); ++v;	// ��
		SetV(v, maxPos.x, minPos.y, minPos.z, 1.0f, 0.0f, -Vector3::UnitY); ++v;	// ��
		SetV(v, maxPos.x, minPos.y, maxPos.z, 1.0f, 1.0f, -Vector3::UnitY); ++v;	// ��
		SetI(i, beginVertexIndex + 16); i += 6;

		// �� (Y+)(Z+ ��UV�̏����)
		SetV(v, minPos.x, maxPos.y, maxPos.z, 0.0f, 0.0f, Vector3::UnitY); ++v;	// ��
		SetV(v, minPos.x, maxPos.y, minPos.z, 0.0f, 1.0f, Vector3::UnitY); ++v;	// ��
		SetV(v, maxPos.x, maxPos.y, maxPos.z, 1.0f, 0.0f, Vector3::UnitY); ++v;	// ��
		SetV(v, maxPos.x, maxPos.y, minPos.z, 1.0f, 1.0f, Vector3::UnitY); ++v;	// ��
		SetI(i, beginVertexIndex + 20);

		if (!m_transform.IsIdentity())
			MeshHelper::Transform(outVertices, v, m_transform);
	}

private:
	Vector3	m_size;
};



class RegularSphereMeshFactory
	: public MeshFactoryBase
{
public:
	RegularSphereMeshFactory()
		: m_radius(0)
		, m_slices(0)
		, m_stacks(0)
	{
	}

	void Initialize(float radius, int slices, int stacks, const Color& color, const Matrix& transform)
	{
		LN_FAIL_CHECK_ARG(slices >= 3) return;
		LN_FAIL_CHECK_ARG(stacks >= 2) return;
		m_radius = radius;
		m_slices = slices;
		m_stacks = stacks;
		MakeSinCosTable();
		MeshFactoryBase::Initialize(color, transform);
	}

	// Squashed

	int GetVertexCount() const
	{
		return (m_slices + 1) * (m_stacks + 1);
		//return 2 + m_slices * (m_stacks - 1);	// (top �� bottom �� 2 �_) + �����O�̒��_�� * �d�˂鐔
	}

	int GetIndexCount() const
	{
		return m_slices * m_stacks * 6;
		//return 2 * m_slices + (m_stacks - 2) * (2 * m_slices);
	}

	//typedef uint16_t Face[3];
	//typedef uint16_t QuadFace[6];

	struct SinCos
	{
		float	sin;
		float	cos;
	};

	void Generate(Vertex* outVertices, uint16_t* outIndices, uint16_t beginIndex)
	{
		Vertex* v = outVertices;
		uint16_t* i = (uint16_t*)outIndices;

		float stackUVStep = 1.0f / m_stacks;
		float stackUV = 0.0f;

		float sliceUVStep = 1.0f / m_slices;

		// XY ���ʏ�� step
		float theta_step = Math::PI / m_stacks;
		float theta = 0.0f;//theta_step;

		// Z+ ���N�_�Ƃ��AX- �����։���Ă���

		// rings (Vertex)
		for (int iStack = 0; iStack < m_stacks + 1; ++iStack)
		{
			float sin_theta = sinf(theta);
			float cos_theta = cosf(theta);
			float sliceUV = 0.0f;

			for (int iSlice = 0; iSlice < m_slices + 1; ++iSlice)
			{
				// top
				if (iStack == 0)
				{
					v->normal.x = 0.0f;
					v->normal.y = 1.0f;
					v->normal.z = 0.0f;
				}
				// bottom
				else if (iStack == m_stacks)
				{
					v->normal.x = 0.0f;
					v->normal.y = -1.0f;
					v->normal.z = 0.0f;
				}
				// between
				else
				{
					v->normal.x = sin_theta * m_sincosTable[iSlice].cos;
					v->normal.y = cos_theta;
					v->normal.z = sin_theta * m_sincosTable[iSlice].sin;
				}
				v->position = v->normal * m_radius;
				v->uv.x = sliceUV;
				v->uv.y = stackUV;
				v->color = m_color;
				++v;

				sliceUV += sliceUVStep;
			}

			stackUV += stackUVStep;
			theta += theta_step;
		}

		// faces
		for (int iStack = 0; iStack < m_stacks; ++iStack)
		{
			for (int iSlice = 0; iSlice < m_slices; ++iSlice)
			{
				int p1 = (iSlice + 0) + (iStack + 0) * (m_slices + 1);	// ��
				int p2 = (iSlice + 0) + (iStack + 1) * (m_slices + 1);	// ��
				int p3 = (iSlice + 1) + (iStack + 0) * (m_slices + 1);	// ��
				int p4 = (iSlice + 1) + (iStack + 1) * (m_slices + 1);	// ��
				i[0] = beginIndex + p1;
				i[1] = beginIndex + p2;
				i[2] = beginIndex + p3;
				i[3] = beginIndex + p3;
				i[4] = beginIndex + p2;
				i[5] = beginIndex + p4;
				i += 6;
			}
		}

		if (!m_transform.IsIdentity())
			MeshHelper::Transform(outVertices, v, m_transform);
	}

	static uint16_t vertex_index(int slices, int slice, int stack)
	{
		return stack*slices + slice + 1;
	}

	void MakeSinCosTable()
	{
		float phi_start = Math::PI / 2.0f;
		float phi_step = -2.0f * Math::PI / m_slices;
		m_sincosTable.Resize(m_slices + 1);

		float angle = phi_start;
		for (int i = 0; i < m_slices; ++i)
		{
			m_sincosTable[i].sin = std::sinf(angle);
			m_sincosTable[i].cos = std::cosf(angle);
			angle -= phi_step;
		}

		m_sincosTable[m_slices] = m_sincosTable[0];
	}

private:
	float	m_radius;
	int		m_slices;
	int		m_stacks;

	List<SinCos>	m_sincosTable;
};

// Sphere ���l�Auv �W�J�̓s���� ring �̎n�_�ƏI�_�A��̈ʒu�͓���B
class RegularCylinderMeshFactory
	: public MeshFactoryBase
{
public:
	RegularCylinderMeshFactory()
		: m_radius(0)
		, m_height(0)
		, m_slices(0)
		, m_stacks(0)
	{
	}

	void Initialize(float radius, float height, int slices, int stacks, const Color& color, const Matrix& transform)
	{
		LN_FAIL_CHECK_ARG(slices >= 3) return;
		LN_FAIL_CHECK_ARG(stacks >= 1) return;
		m_radius = radius;
		m_height = height;
		m_slices = slices;
		m_stacks = stacks;
		MeshFactoryBase::Initialize(color, transform);
	}

	int GetVertexCount() const
	{
		return (m_slices + 1) * (m_stacks + 3);
	}

	int GetIndexCount() const
	{
		return m_slices * (m_stacks + 2) * 6;
	}

	void Generate(Vertex* outVertices, uint16_t* outIndices, uint16_t beginIndex)
	{
		Vertex* vb = outVertices;
		uint16_t* ib = (uint16_t*)outIndices;

		float yStep = m_height / m_stacks;
		float y;
		float yu = m_height / 2;
		float yd = -m_height / 2;

		//for (int iSlice = 0; iSlice < m_slices + 1; ++iSlice)
		for (int iSlice = m_slices; iSlice >= 0; iSlice--)
		{
			Vector3 n = MeshHelper::GetXZCirclePoint(iSlice, m_slices);
			Vector3 xz = n * m_radius;

			// upper base
			{
				Vertex v;
				v.position.Set(0, yu, 0);
				v.normal = Vector3::UnitY;
				v.color = m_color;
				AddVertex(&vb, v);
			}
			// side
			y = yu;
			for (int iStack = 0; iStack < m_stacks + 1; ++iStack)
			{
				Vertex v;
				v.position.x = xz.x;
				v.position.y = y;
				v.position.z = xz.z;
				v.normal = n;
				v.color = m_color;
				AddVertex(&vb, v);
				y -= yStep;
			}
			// lower base
			{
				Vertex v;
				v.position.Set(0, yd, 0);
				v.normal = -Vector3::UnitY;
				v.color = m_color;
				AddVertex(&vb, v);
			}
		}

		// faces
		int stacks = m_stacks + 2;
		for (int iSlice = 0; iSlice < m_slices; ++iSlice)	// x
		{
			for (int iStack = 0; iStack < stacks; ++iStack)	// y
			{
				int p1 = (iStack + 0) + (iSlice + 0) * (stacks + 1);	// ��
				int p2 = (iStack + 1) + (iSlice + 0) * (stacks + 1);	// ��
				int p3 = (iStack + 0) + (iSlice + 1) * (stacks + 1);	// ��
				int p4 = (iStack + 1) + (iSlice + 1) * (stacks + 1);	// ��
				ib[0] = beginIndex + p1;
				ib[1] = beginIndex + p2;
				ib[2] = beginIndex + p3;
				ib[3] = beginIndex + p3;
				ib[4] = beginIndex + p2;
				ib[5] = beginIndex + p4;
				ib += 6;
			}
		}

		if (!m_transform.IsIdentity())
			MeshHelper::Transform(outVertices, vb, m_transform);
	}

	static void AddVertex(Vertex** vb, const Vertex& v)
	{
		*(*vb) = v;
		(*vb)++;
	}

private:
	float	m_radius;
	float	m_height;
	int		m_slices;
	int		m_stacks;
};


class RegularConeMeshFactory
	: public MeshFactoryBase
{
public:
	RegularConeMeshFactory()
		: m_radius(0)
		, m_height(0)
		, m_slices(0)
	{
	}

	void Initialize(float radius, float height, int slices, const Color& color, const Matrix& transform)
	{
		LN_FAIL_CHECK_ARG(slices >= 3) return;
		m_radius = radius;
		m_height = height;
		m_slices = slices;
		MeshFactoryBase::Initialize(color, transform);
	}

	int GetVertexCount() const
	{
		return (m_slices + 1) * 3;
	}

	int GetIndexCount() const
	{
		return m_slices * 3 * 6;
	}

	void Generate(Vertex* outVertices, uint16_t* outIndices, uint16_t beginIndex)
	{
		Vertex* vb = outVertices;
		uint16_t* ib = (uint16_t*)outIndices;

		for (int iSlice = m_slices; iSlice >= 0; iSlice--)
		{
			Vector3 n = MeshHelper::GetXZCirclePoint(iSlice, m_slices);
			Vector3 xz = n * m_radius;

			// top
			{
				Vertex v;
				v.position.Set(0, m_height / 2, 0);
				v.normal = Vector3::UnitY;
				v.color = m_color;
				AddVertex(&vb, v);
			}
			// side
			float y = -m_height / 2;
			{
				Vertex v;
				v.position.Set(xz.x, y, xz.z);
				v.normal = n;
				v.color = m_color;
				AddVertex(&vb, v);
			}
			// lower base
			{
				Vertex v;
				v.position.Set(0, y, 0);
				v.normal = -Vector3::UnitY;
				v.color = m_color;
				AddVertex(&vb, v);
			}
		}

		// faces
		int stacks = 2;
		for (int iSlice = 0; iSlice < m_slices; ++iSlice)	// x
		{
			for (int iStack = 0; iStack < stacks; ++iStack)	// y
			{
				int p1 = (iStack + 0) + (iSlice + 0) * (stacks + 1);	// ��
				int p2 = (iStack + 1) + (iSlice + 0) * (stacks + 1);	// ��
				int p3 = (iStack + 0) + (iSlice + 1) * (stacks + 1);	// ��
				int p4 = (iStack + 1) + (iSlice + 1) * (stacks + 1);	// ��
				ib[0] = beginIndex + p1;
				ib[1] = beginIndex + p2;
				ib[2] = beginIndex + p3;
				ib[3] = beginIndex + p3;
				ib[4] = beginIndex + p2;
				ib[5] = beginIndex + p4;
				ib += 6;
			}
		}

		if (!m_transform.IsIdentity())
			MeshHelper::Transform(outVertices, vb, m_transform);
	}

	static void AddVertex(Vertex** vb, const Vertex& v)
	{
		*(*vb) = v;
		(*vb)++;
	}

private:
	float	m_radius;
	float	m_height;
	int		m_slices;
};


class ArcMeshFactory
	: public MeshFactoryBase
{
public:
	ArcMeshFactory()
		: m_startAngle(0)
		, m_endAngle(0)
		, m_innerRadius(0)
		, m_outerRadius(0)
		, m_slices(0)
	{
	}

	void Initialize(float startAngle, float endAngle, float innerRadius, float outerRadius, int slices, const Color& color, const Matrix& transform)
	{
		LN_FAIL_CHECK_ARG(slices >= 1) return;
		m_startAngle = startAngle;
		m_endAngle = endAngle;
		m_innerRadius = innerRadius;
		m_outerRadius = outerRadius;
		m_slices = slices;
		MeshFactoryBase::Initialize(color, transform);
	}

	int GetVertexCount() const
	{
		return (m_slices + 1) * 2;
	}

	int GetIndexCount() const
	{
		return m_slices * 6;
	}

	void Generate(Vertex* outVertices, uint16_t* outIndices, uint16_t beginIndex)
	{
		Vertex* vb = outVertices;
		uint16_t* ib = (uint16_t*)outIndices;

		for (int iSlice = 0; iSlice < m_slices + 1; iSlice++)
		{
			Vector3 n = MeshHelper::GetXZArcPoint(m_startAngle, m_endAngle, iSlice, m_slices);

			// outer
			{
				Vertex v;
				v.position = n * m_outerRadius;
				v.normal = Vector3::UnitY;
				v.color = m_color;
				AddVertex(&vb, v);
			}
			// inner
			{
				Vertex v;
				v.position = n * m_innerRadius;
				v.normal = Vector3::UnitY;
				v.color = m_color;
				AddVertex(&vb, v);
			}
		}

		// faces
		for (int iSlice = 0; iSlice < m_slices; iSlice++)
		{
			int p1 = ((iSlice + 0) * 2 + 0);	// ��
			int p2 = ((iSlice + 0) * 2 + 1);	// ��
			int p3 = ((iSlice + 1) * 2 + 0);	// ��
			int p4 = ((iSlice + 1) * 2 + 1);	// ��
			ib[0] = beginIndex + p1;
			ib[1] = beginIndex + p2;
			ib[2] = beginIndex + p3;
			ib[3] = beginIndex + p3;
			ib[4] = beginIndex + p2;
			ib[5] = beginIndex + p4;
			ib += 6;
		}

		if (!m_transform.IsIdentity())
			MeshHelper::Transform(outVertices, vb, m_transform);
	}

	static void AddVertex(Vertex** vb, const Vertex& v)
	{
		*(*vb) = v;
		(*vb)++;
	}

private:
	float	m_startAngle;
	float	m_endAngle;
	float 	m_innerRadius;
	float	m_outerRadius;
	int		m_slices;
};

} // namespace detail
LN_NAMESPACE_END