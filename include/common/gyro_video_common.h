#ifndef GYRO_VIDEO_COMMON_H
#define GYRO_VIDEO_COMMON_H

static inline void gyVidOrthoProjMatrix(GYMatrix4 mat, const float pleft, const float pright,
						 const float pbottom, const float ptop,
						 const float pnear, const float pfar) {
	const GYMatrix4 orthoProjMat = {
		{ 2.0f/(pright-pleft), 0.0f, 0.0f, -(pright+pleft)/(pright-pleft) },
		{ 0.0f, 2.0f/(ptop-pbottom), 0.0f, -(ptop+pbottom)/(ptop-pbottom) },
		{ 0.0f, 0.0f, -2.0f/(pfar-pnear), -(pfar+pnear)/(pfar-pnear) },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};

	for(unsigned i = 0; i < 4; ++i)
		for(unsigned j = 0; j < 4; ++j)
			mat[i][j] = orthoProjMat[j][i];

}

#endif // GYRO_VIDEO_COMMON_H
