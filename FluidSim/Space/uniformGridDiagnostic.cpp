#include "Space\uniformGrid.h"

/*static*/ void UniformGrid<unsigned>::UnitTest(void)
{
	/*
	1. ��һ����Χ���������Position
	2. ����Postion��ʼ��UniformGrid	���UniformGrid�Ļ�����Ϣ
	3. Ϊÿһ��Position �� itemT Ϊ��Position idx
	4. Iterate through each cell in the UniformGrid and obtains its contents

		��ÿһ��cell ��minCorner maxCorner �ֱ���һ��vPosMin, vPosMax

		1 ���offsetFromIndices�Ƿ���ȷ
		2 ���vPosMin' indices, offset �Ƿ���ȷ

	*/

	static unsigned const num = 1024;
	static Vector3D vRange(2.0f, 3.0f, 5.0f);

	{
	
		Vector3D vPositions[num];

		/*The actual range of the positions*/
		Vector3D vMin(1.0e8f, 1.0e8f, 1.0e8f);
		Vector3D vMax(-1.0e8f, -1.0e8f, -1.0e8f);

		/*ע�⣬����Ҳ���Կ�����uniformGrid ctor ��vMin vMax ������Bounding Box�ľ��巶Χ��ֵ
		���Ǿ����point position*/
		for (unsigned idx = 0; idx < num; idx++)
		{
			vPositions[idx].x = vRange.x * (float(rand()) / float(RAND_MAX) - 0.5f);
			vPositions[idx].y = vRange.y * (float(rand()) / float(RAND_MAX) - 0.5f);
			vPositions[idx].z = vRange.z * (float(rand()) / float(RAND_MAX) - 0.5f);

			vMin.x = MIN2(vMin.x, vPositions[idx].x);
			vMin.y = MIN2(vMin.y, vPositions[idx].y);
			vMin.z = MIN2(vMin.z, vPositions[idx].z);

			vMax.x = MAX2(vMax.x, vPositions[idx].x);
			vMax.y = MAX2(vMax.y, vPositions[idx].y);
			vMax.z = MAX2(vMax.z, vPositions[idx].z);
		}

		UniformGrid<unsigned> ugm(num, vMin, vMax, true);
		ugm.Init();

		for (unsigned idx = 0; idx < num; idx++)
		{
			ugm[vPositions[idx]] = idx;
		}

		unsigned capacity = ugm.GetNumPoints(0) * ugm.GetNumPoints(1) * ugm.GetNumPoints(2);
		/*���ά�� ���� CellSpacing*/
		fprintf(stderr, "dims=%u, %u, %u, cap=%u, spacing={%f, %f, %f}\n",
			ugm.GetNumPoints(0), ugm.GetNumPoints(1), ugm.GetNumPoints(2),
			capacity,
			ugm.GetCellSpacing().x, ugm.GetCellSpacing().y, ugm.GetCellSpacing().z);

		/*ͨ���ֶ����ɵ�indices���������cell����ÿһ��cell ��min max corner ����
		ѡȡ������vPosMin vPosMax, ���assert vPosMin, vPosMax ��cell ��*/

		unsigned idx[3];
		const unsigned numXY = ugm.GetNumPoints(0) * ugm.GetNumPoints(1);
		for (idx[2] = 0; idx[2] < ugm.GetNumPoints(2); idx[2]++)
		{
			/*near by, min max corner ��ȡһ��С��ƫ��*/
			const Vector3D Nudge = 2.0f * FLT_EPSILON * ugm.GetExtent();
			Vector3D vPosMin;
			vPosMin.z = vMin.z + float(idx[2]    ) * ugm.GetCellSpacing().z + Nudge.z;
			Vector3D vPosMax;
			vPosMax.z = vMin.z + float(idx[2] + 1) * ugm.GetCellSpacing().z - Nudge.z;
			/*offset*/
			const unsigned offsetZ = idx[2] * numXY;

			for (idx[1] = 0; idx[1] < ugm.GetNumPoints(1); idx[1]++)
			{
				vPosMin.y = vMin.y + float(idx[1]    ) * ugm.GetCellSpacing().y + Nudge.y;
				vPosMax.y = vMin.y + float(idx[1] + 1) * ugm.GetCellSpacing().y - Nudge.y;
				/*offset*/
				const unsigned offsetYZ = offsetZ + idx[1] * ugm.GetNumPoints(0);

				for (idx[0] = 0; idx[0] < ugm.GetNumPoints(0); idx[0]++)
				{
					vPosMin.x = vMin.x + float(idx[0]    ) * ugm.GetCellSpacing().x + Nudge.x;
					vPosMax.x = vMin.x + float(idx[0] + 1) * ugm.GetCellSpacing().x - Nudge.x;
					/*offset*/
					const unsigned offsetXYZ = offsetYZ + idx[0];
					_ASSERT(ugm.OffsetFromIndices(idx) == offsetXYZ);

					/*contents from offset*/
					unsigned & contentsFromOffset = ugm[offsetXYZ];
					//vPosMin
					{
						//indices
						unsigned indices[3];
						ugm.IndicesOfPosition(indices, vPosMin);
						_ASSERT(indices[0] == idx[0] && indices[1] == idx[1] && indices[2] == idx[2]);
						//offset
						_ASSERT(ugm.OffsetOfPosition(vPosMin) == offsetXYZ);
						/*contents from position*/
						unsigned & contentsFromPos = ugm[vPosMin];
						_ASSERT(contentsFromOffset == contentsFromPos);

					}

					//vPosMax
					{
						//indices
						unsigned indices[3];
						ugm.IndicesOfPosition(indices, vPosMax);
						_ASSERT(indices[0] == idx[0] && indices[1] == idx[1] && indices[2] == idx[2]);
						//offset
						_ASSERT(ugm.OffsetOfPosition(vPosMax) == offsetXYZ);
						/*contents from position*/
						unsigned & contentsFromPos = ugm[vPosMax];
						_ASSERT(contentsFromOffset == contentsFromPos);
					}
				}
			}
		}

	}
	

	/*
	��֮ǰ���ƣ���μ��ÿ��cell �����ĵ�
	*/
	{
		Vector3D vMin(-0.5f * vRange);
		Vector3D vMax( 0.5f * vRange);
		UniformGrid<unsigned> ugm2(num, vMin, vMax, true);
		Vector3D vSpacing( vRange.x / float(ugm2.GetNumCells(0)), 
			vRange.y / float(ugm2.GetNumCells(1)),
			vRange.z / float(ugm2.GetNumCells(2)));
		_ASSERT(vSpacing.Resembles(ugm2.GetCellSpacing()));

		unsigned idx[3];
		const Vector3D vMidOffset(ugm2.GetCellSpacing() / float(2));
		unsigned counter = 0;
		for ( idx[2] = 0; idx[2] < ugm2.GetNumPoints(2); idx[2]++)
		{
			Vector3D vMidPoint;
			vMidPoint.z = vMin.z + float(idx[2]) * ugm2.GetCellSpacing().z + vMidOffset.z;

			for ( idx[1] = 0; idx[1] < ugm2.GetNumPoints(1); idx[1]++)
			{
				vMidPoint.y = vMin.y + float(idx[1]) * ugm2.GetCellSpacing().y + vMidOffset.y;

				for (idx[0] = 0; idx[0] < ugm2.GetNumPoints(0); idx[0]++)
				{
					vMidPoint.x = vMin.x + (idx[0]) * ugm2.GetCellSpacing().x + vMidOffset.x;

					unsigned indices[3];
					ugm2.IndicesOfPosition(indices, vMidPoint);
					_ASSERT(indices[0] == idx[0]);
					_ASSERT(indices[1] == idx[1]);
					_ASSERT(indices[2] == idx[2]);
					
					Vector3D vPosCheck;
					ugm2.PositionFromIndices(vPosCheck, indices);
					vPosCheck += vMidOffset;
					_ASSERT(vPosCheck.Resembles(vMidPoint));
					counter++;
				}
			}
		}

		_ASSERT(counter == ugm2.GetNumPoints(0) * ugm2.GetNumPoints(1) * ugm2.GetNumPoints(2));
	}


	/* ���low level based container ����
		��֤��numCells Ϊ ԭnumCells / iDecimation
	*/
	{
		Vector3D vMin(-0.5f * vRange);
		Vector3D vMax( 0.5f * vRange);
		UniformGrid<unsigned> ugma(num, vMin, vMax, true);
		UniformGrid<unsigned> ugmb;
		int iDecimation = 2;
		ugmb.Decimate(ugma, iDecimation);
		
		_ASSERT(ugma.GetNumCells(0) / 2 == ugmb.GetNumCells(0));
		_ASSERT(ugma.GetNumCells(1) / 2 == ugmb.GetNumCells(1));
		_ASSERT(ugma.GetNumCells(2) / 2 == ugmb.GetNumCells(2));

	}

}