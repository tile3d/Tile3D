#pragma once

//	Identity and version of terrain2 general information file
const int TRN2FILE_IDENTIFY = (('T' << 24) | ('R' << 16) | ('N' << 8) | '2');
const int TRN2FILE_VERSION = 1;

//	Identity and version of terrain2 light map file
const int T2LMFILE_IDENTIFY = (('T' << 24) | ('2' << 16) | ('L' << 8) | 'M');
const int T2LMFILE_VERSION = 2;

//	Identity and version of terrain2 block data file
const int T2BKFILE_IDENTIFY = (('T' << 24) | ('B' << 16) | ('K' << 8) | 'B');
const int T2BKFILE_VERSION = 6;

//	Identity and version of terrain2 mask map file
const int T2MKFILE_IDENTIFY = (('T' << 24) | ('M' << 16) | ('K' << 8) | 'B');
const int T2MKFILE_VERSION = 3;

//	Terrain file ID and cersion
struct TRN2FILEIDVER
{
	int	m_identify;		//	Identity, must be TRN2FILE_IDENTIFY
	int	m_version;		//	Version, must be TRN2FILE_VERSION
};

//	File header when TRN2FILE_VERSION == 1
struct TRN2FILEHEADER
{
	float	m_terrainWid;	//	Terrain width in meters
	float	m_terrainHei;	//	Terrain height in meters
	float	m_blockSize;	//	Block size in meters
	int		m_blockGrid;	//	Block grid row and column number, each block has (grid x grid) terrain grids
	int		m_numBlock;		//	Number of block
	int		m_trnBlkRow;	//	Block row number in whole terrain
	int		m_trnBlkCol;	//	Block column number in whole terrain
	int		m_numTexture;	//	Number of texture
	int		m_numMaskArea;	//	Number of mask area
	int		m_maskBlockNum;	//	Each mask area has iMaskBlock * iMaskBlock blocks
	int		m_maskGridNum;	//	Mask grid row and column number, each mask area has (grid x grid) terrain grids
	int		m_numSubTrn;	//	Number of sub-terrain
	int		m_subTrnRow;	//	Row number of sub-terrain
	int		m_subTrnCol;	//	Column number of sub-terrain
	int		m_subTrnGrid;	//	Each sub-terrain has iSubTrnGrid * iSubTrnGrid grids
	float	m_gridSize;		//	Terrain grid size in meters
	int		m_texOffset;	//	Texture file name list's offset in file
	int		m_loaderID;		//	Terrain loader ID
	int		m_loaderOff;	//	Offset of loader's data in file
	char	m_reserved[56];	//	Reserved
};

//	Sub-terrain information
struct TRN2FILESUBTRN
{
	float	m_minHei;		//	Minimum height
	float	m_maxHei;		//	Maximum height
};

