#pragma once
#include <cstdint>
#include <Vector3.h>
#include <Quaternion.h>
#include <corecrt_math.h>
#include <Vector2.h>
#include "ListBase.h"
#include "StringBase.h"

typedef unsigned short uint16;


struct rmdlflags_t
{
	// This flag is set if no hitbox information was specified
	int STUDIOHDR_FLAGS_AUTOGENERATED_HITBOX : 1;

	// NOTE:  This flag is set at loadtime, not mdl build time so that we don't have to rebuild
	// models when we change materials.
	int STUDIOHDR_FLAGS_USES_ENV_CUBEMAP : 1;

	// Use this when there are translucent parts to the model but we're not going to sort it 
	int STUDIOHDR_FLAGS_FORCE_OPAQUE : 1;

	// Use this when we want to render the opaque parts during the opaque pass
	// and the translucent parts during the translucent pass
	int STUDIOHDR_FLAGS_TRANSLUCENT_TWOPASS : 1;

	// This is set any time the .qc files has $staticprop in it
	// Means there's no bones and no transforms
	int STUDIOHDR_FLAGS_STATIC_PROP : 1;

	// NOTE:  This flag is set at loadtime, not mdl build time so that we don't have to rebuild
	// models when we change materials.
	int STUDIOHDR_FLAGS_USES_FB_TEXTURE : 1;

	// This flag is set by studiomdl.exe if a separate "$shadowlod" entry was present
	//  for the .mdl (the shadow lod is the last entry in the lod list if present)
	int STUDIOHDR_FLAGS_HASSHADOWLOD : 1;

	// NOTE:  This flag is set at loadtime, not mdl build time so that we don't have to rebuild
	// models when we change materials.
	int STUDIOHDR_FLAGS_USES_BUMPMAPPING : 1;

	// NOTE:  This flag is set when we should use the actual materials on the shadow LOD
	// instead of overriding them with the default one (necessary for translucent shadows)
	int STUDIOHDR_FLAGS_USE_SHADOWLOD_MATERIALS : 1;

	// NOTE:  This flag is set when we should use the actual materials on the shadow LOD
	// instead of overriding them with the default one (necessary for translucent shadows)
	int STUDIOHDR_FLAGS_OBSOLETE : 1;

	int STUDIOHDR_FLAGS_UNUSED : 1;

	// NOTE:  This flag is set at mdl build time
	int STUDIOHDR_FLAGS_NO_FORCED_FADE : 1;

	// NOTE:  The npc will lengthen the viseme check to always include two phonemes
	int STUDIOHDR_FLAGS_FORCE_PHONEME_CROSSFADE : 1;

	// This flag is set when the .qc has $constantdirectionallight in it
	// If set, we use constantdirectionallightdot to calculate light intensity
	// rather than the normal directional dot product
	// only valid if STUDIOHDR_FLAGS_STATIC_PROP is also set
	int STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT : 1;

	// Flag to mark delta flexes as already converted from disk format to memory format
	int STUDIOHDR_FLAGS_FLEXES_CONVERTED : 1;

	// Indicates the studiomdl was built in preview mode
	int STUDIOHDR_FLAGS_BUILT_IN_PREVIEW_MODE : 1;

	// Ambient boost (runtime flag)
	int STUDIOHDR_FLAGS_AMBIENT_BOOST : 1;

	// Don't cast shadows from this model (useful on first-person models)
	int STUDIOHDR_FLAGS_DO_NOT_CAST_SHADOWS : 1;

	// alpha textures should cast shadows in vrad on this model (ONLY prop_static!)
	int STUDIOHDR_FLAGS_CAST_TEXTURE_SHADOWS : 1;

	// Model has a quad-only Catmull-Clark SubD cage
	int STUDIOHDR_FLAGS_SUBDIVISION_SURFACE : 1;

	// flagged on load to indicate no animation events on this model
	int STUDIOHDR_FLAGS_NO_ANIM_EVENTS : 1;

	// If flag is set then studiohdr_t.flVertAnimFixedPointScale contains the
	// scale value for fixed point vert anim data, if not set then the
	// scale value is the default of 1.0 / 4096.0.  Regardless use
	// studiohdr_t::VertAnimFixedPointScale() to always retrieve the scale value
	int STUDIOHDR_FLAGS_VERT_ANIM_FIXED_POINT_SCALE : 1;

	int unused : 1; // likely exists

	int unk : 1; // known used in v53, used a lot on props
	int unk1 : 1; // known used in v52 'envball.mdl'
	int unk2 : 1; // seen used in v54 'ptpov_doubletake.rmdl'

	int unused4 : 1;
	int unused5 : 1;
	int unused6 : 1;
	int unused7 : 1;
	int unused8 : 1;
	int unused9 : 1;
};

#pragma pack(push, 1)
struct matrix3x4_t
{
	// unsure if that's how it actually works

	// row 1, x
	float c0r0; // x
	float c1r0; // x
	float c2r0; // x
	float c3r0; // x

	// row 2, y
	float c0r1; // y
	float c1r1; // y
	float c2r1; // y
	float c3r1; // y

	// row 3, z
	float c0r2; // z
	float c1r2; // z
	float c2r2; // z
	float c3r2; // z

	Vector3 GetRotationMatrixAsDegrees()
	{
		// c0r0, c1r0, c2r0, c0r1, c1r1, c2r1, c2r2
		float x, y, z, tX, tY;

		float m2 = c2r0;
		if (m2 < -1)
			m2 = -1;
		else if (m2 > 1)
			m2 = 1;

		y = -asin(m2);

		printf("%f\n", m2);

		float cosY = cos(y);

		y = Math::MathHelper::RadiansToDegrees(y);

		if (abs(cosY) > 0.005)
		{
			tX = c2r2 / cosY;
			tY = -c2r1 / cosY;
			x = Math::MathHelper::RadiansToDegrees(atan2(tY, tX));

			tX = c0r0 / cosY;
			tY = -c1r0 / cosY;
			z = Math::MathHelper::RadiansToDegrees(atan2(tY, tX));
		}
		else
		{
			x = 0;
			tX = c1r1 / cosY;
			tY = c0r1 / cosY;
			z = Math::MathHelper::RadiansToDegrees(atan2(tY, tX));
		}


		// these can take some strange orders and idrk how to determine it
		//if (m2 == 0)
		//{
		//	return { x,z,y };
		//}
		//else if (m2 < 1 && m2 != -1)
		//{
		//	return { z,y,x };
		//}

		return { x,y,z };
	}
};

struct studiohdr_t // latest studiohdr
{
	uint32_t Magic;
	uint32_t Version;
	uint32_t Hash;
	uint32_t NameTableOffset;

	char SkeletonName[0x40];

	uint32_t DataSize;

	float EyePosition[3];
	float IllumPosition[3];
	float HullMin[3];
	float HullMax[3];
	float ViewBBMin[3];
	float ViewBBMax[3];

	uint32_t Flags; // 0x9c

	uint32_t BoneCount; // 0xa0
	uint32_t BoneDataOffset; // 0xa4

	uint32_t BoneControllerCount;
	uint32_t BoneControllerOffset;

	uint32_t HitboxCount;
	uint32_t HitboxOffset;

	uint32_t LocalAnimCount;
	uint32_t LocalAnimOffset;

	uint32_t LocalSeqCount;
	uint32_t LocalSeqOffset;

	uint32_t ActivityListVersion;
	uint32_t EventsIndexed;

	uint32_t TextureCount;
	uint32_t TextureOffset;

	uint32_t TextureDirCount;
	uint32_t TextureDirOffset;

	uint32_t SkinReferenceCount;	// Total number of references (submeshes)
	uint32_t SkinFamilyCount;		// Total skins per reference
	uint32_t SkinReferenceOffset;	// Offset to data

	uint32_t BodyPartCount;
	uint32_t BodyPartOffset;

	uint32_t AttachmentCount;
	uint32_t AttachmentOffset;

	uint8_t Unknown2[0x14];

	uint32_t NahhhO;
	uint32_t SubmeshLodsOffset;

	uint32_t Unk;
	uint32_t SubmeshLodsOffset_V14;

	uint8_t Unknown3[0x3C];
	uint32_t OffsetToBoneRemapInfo;
	uint32_t BoneRemapCount;
	uint32_t OffsetToBoneRemapInfo_V14;
	uint32_t BoneRemapCount_V14;
};

struct s3studiohdr_t // season 3 studiohdr
{
	int id; // Model format ID, such as "IDST" (0x49 0x44 0x53 0x54)
	int version; // Format version number, such as 48 (0x30,0x00,0x00,0x00)
	int checksum; // This has to be the same in the phy and vtx files to load!
	int sznameindex; // This has been moved from studiohdr2 to the front of the main header.
	char name[64]; // The internal name of the model, padding with null bytes.
	// Typically "my_model.mdl" will have an internal name of "my_model"
	int length; // Data size of MDL file in bytes.

	Vector3 eyeposition;	// ideal eye position

	Vector3 illumposition;	// illumination center

	Vector3 hull_min;		// ideal movement hull size
	Vector3 hull_max;

	Vector3 view_bbmin;		// clipping bounding box
	Vector3 view_bbmax;

	rmdlflags_t flags;

	int numbones; // bones
	int boneindex;

	int numbonecontrollers; // bone controllers
	int bonecontrollerindex;

	int numhitboxsets;
	int hitboxsetindex;

	int numlocalanim; // animations/poses
	int localanimindex; // animation descriptions

	int numlocalseq; // sequences
	int	localseqindex;

	int activitylistversion; // initialization flag - have the sequences been indexed?

	// mstudiotexture_t
	// short rpak path
	// raw textures
	int materialtypesindex;
	int numtextures; // the material limit exceeds 128, probably 256.
	int textureindex;

	// this should always only be one, unless using vmts.
	// raw textures search paths
	int numcdtextures;
	int cdtextureindex;

	// replaceable textures tables
	int numskinref;
	int numskinfamilies;
	int skinindex;

	int numbodyparts;
	int bodypartindex;

	int numlocalattachments;
	int localattachmentindex;

	int numlocalnodes;
	int localnodeindex;
	int localnodenameindex;

	int numflexdesc;
	int flexdescindex;

	int meshindex; // SubmeshLodsOffset, might just be a mess offset

	int numflexcontrollers;
	int flexcontrollerindex;

	int numflexrules;
	int flexruleindex;

	int numikchains;
	int ikchainindex;

	// this is rui meshes
	int numruimeshes;
	int ruimeshindex;

	int numlocalposeparameters;
	int localposeparamindex;

	int surfacepropindex;

	int keyvalueindex;
	int keyvaluesize;

	int numlocalikautoplaylocks;
	int localikautoplaylockindex;

	float mass;
	int contents;

	// unused for packed models
	int numincludemodels;
	int includemodelindex;

	uint32_t virtualModel;

	int bonetablebynameindex;

	// if STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT is set,
	// this value is used to calculate directional components of lighting 
	// on static props
	byte constdirectionallightdot;

	// set during load of mdl data to track *desired* lod configuration (not actual)
	// the *actual* clamped root lod is found in studiohwdata
	// this is stored here as a global store to ensure the staged loading matches the rendering
	byte rootLOD;

	// set in the mdl data to specify that lod configuration should only allow first numAllowRootLODs
	// to be set as root LOD:
	//	numAllowedRootLODs = 0	means no restriction, any lod can be set as root lod.
	//	numAllowedRootLODs = N	means that lod0 - lod(N-1) can be set as root lod, but not lodN or lower.
	byte numAllowedRootLODs;

	byte unused;

	float fadedistance;

	float gathersize; // what. from r5r struct

	int numunk_v54_early;
	int unkindex_v54_early;

	int unk_v54[2];

	// this is in all shipped models, probably part of their asset bakery. it should be 0x2CC.
	int mayaindex; // doesn't actually need to be written pretty sure, only four bytes when not present.

	int numsrcbonetransform;
	int srcbonetransformindex;

	int	illumpositionattachmentindex;

	int linearboneindex;

	int m_nBoneFlexDriverCount; // unsure if that's what it is in apex
	int m_nBoneFlexDriverIndex;

	int unk1_v54[7];

	// always "" or "Titan"
	int unkstringindex;

	// this is now used for combined files in rpak, vtx, vvd, and vvc are all combined while vphy is separate.
	// the indexes are added to the offset in the rpak mdl_ header.
	// vphy isn't vphy, looks like a heavily modified vphy.
	int vtxindex; // VTX
	int vvdindex; // VVD / IDSV
	int vvcindex; // VVC / IDCV 
	int vphyindex; // VPHY / IVPS

	int vtxsize;
	int vvdsize;
	int vvcsize;
	int vphysize; // still used in models using vg

	// unk2_v54[3] is the chunk after following unkindex2's chunk
	int unk2_v54[3]; // the same four unks in v53 I think, the first index being unused now probably

	int unkindex3; // index to chunk after string block

	// Per Tri Collision AABB size
	Vector3 mins;
	Vector3 maxs; // seem to be the same as hull size

	int unk3_v54[3];

	int unkindex4; // chunk before unkindex3 sometimes

	int unk4_v54[3]; // same as unk3_v54_v121

	//int vgindex; // 0tVG
	//int unksize; // might be offset
	//int unksize1; // might be offset

};

struct r2studiohdr_t // titanfall 2 studiohdr (MDL v53)
{
	int id; // Model format ID, such as "IDST" (0x49 0x44 0x53 0x54)
	int version; // Format version number, such as 48 (0x30,0x00,0x00,0x00)
	int checksum; // This has to be the same in the phy and vtx files to load!
	int sznameindex; // This has been moved from studiohdr2 to the front of the main header.
	char name[64]; // The internal name of the model, padding with null bytes.
	// Typically "my_model.mdl" will have an internal name of "my_model"
	int length; // Data size of MDL file in bytes.

	Vector3 eyeposition;	// ideal eye position

	Vector3 illumposition;	// illumination center

	Vector3 hull_min;		// ideal movement hull size
	Vector3 hull_max;

	Vector3 view_bbmin;		// clipping bounding box
	Vector3 view_bbmax;

	int flags;

	int numbones; // bones
	int boneindex;

	int numbonecontrollers; // bone controllers
	int bonecontrollerindex;

	int numhitboxsets;
	int hitboxsetindex;

	int numlocalanim; // animations/poses
	int localanimindex; // animation descriptions

	int numlocalseq; // sequences
	int localseqindex;

	int activitylistversion; // initialization flag - have the sequences been indexed?
	int eventsindexed;

	// mstudiotexture_t
	// short rpak path
	// raw textures
	int numtextures; // the material limit exceeds 128, probably 256.
	int textureindex;

	// this should always only be one, unless using vmts.
	// raw textures search paths
	int numcdtextures;
	int cdtextureindex;

	// replaceable textures tables
	int numskinref;
	int numskinfamilies;
	int skinindex;

	int numbodyparts;
	int bodypartindex;

	int numlocalattachments;
	int localattachmentindex;

	int numlocalnodes;
	int localnodeindex;
	int localnodenameindex;

	int numflexdesc;
	int flexdescindex;

	int numflexcontrollers;
	int flexcontrollerindex;

	int numflexrules;
	int flexruleindex;

	int numikchains;
	int ikchainindex;

	// this is rui meshes, todo refind mouth count.
	int numruimeshes;
	int ruimeshindex;

	int numlocalposeparameters;
	int localposeparamindex;

	int surfacepropindex;

	int keyvalueindex;
	int keyvaluesize;

	int numlocalikautoplaylocks;
	int localikautoplaylockindex;

	float mass;
	int contents;

	// external animations, models, etc.
	int numincludemodels;
	int includemodelindex;

	uint32_t virtualModel;

	// animblock is either completely cut, this is because they no longer use .ani files.

	int bonetablebynameindex;

	// if STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT is set,
	// this value is used to calculate directional components of lighting 
	// on static props
	byte constdirectionallightdot;

	// set during load of mdl data to track *desired* lod configuration (not actual)
	// the *actual* clamped root lod is found in studiohwdata
	// this is stored here as a global store to ensure the staged loading matches the rendering
	byte rootLOD;

	// set in the mdl data to specify that lod configuration should only allow first numAllowRootLODs
	// to be set as root LOD:
	//	numAllowedRootLODs = 0	means no restriction, any lod can be set as root lod.
	//	numAllowedRootLODs = N	means that lod0 - lod(N-1) can be set as root lod, but not lodN or lower.
	byte numAllowedRootLODs;

	byte unused;

	float fadedistance; // set to -1 to never fade. set above 0 if you want it to fade out, distance is in feet.
	// player/titan models seem to inherit this value from the first model loaded in menus.
	// works oddly on entities, probably only meant for static props

	int numflexcontrollerui;
	int flexcontrolleruiindex;

	// used by tools only that don't cache, but persist mdl's peer data
	// engine uses virtualModel to back link to cache pointers
	// might not be correct
	uint32_t pVertexBase; // float flVertAnimFixedPointScale;
	uint32_t pIndexBase; // int surfacepropLookup;

	// this is in all shipped models, probably part of their asset bakery. it should be 0x2CC.
	// doesn't actually need to be written pretty sure, only four bytes when not present.
	// this is not completely true as some models simply have nothing, such as animation models.
	int mayaindex;

	int numsrcbonetransform;
	int srcbonetransformindex;

	int illumpositionattachmentindex;

	int linearboneindex;

	int m_nBoneFlexDriverCount;
	int m_nBoneFlexDriverIndex;

	// for static props (and maybe others)
	// Per Triangle AABB
	int aabbindex;
	int numaabb;
	int numaabb1;
	int numaabb2;

	int stringtableindex;

	// start of model combination stuff.
	// anis are no longer used from what I can tell, v52s that had them don't in v53.
	int vtxindex; // VTX
	int vvdindex; // VVD / IDSV
	int vvcindex; // VVC / IDCV 
	int vphyindex; // VPHY / IVPS

	int vtxsize;
	int vvdsize;
	int vvcsize;
	int vphysize;

	// the following 'unks' could actually be indexs.
	// one of these is probably the ANI/IDAG index
	// vertAnimFixedPointScale might be in here but I doubt it.

	// this data block is related to the vphy, if it's not present the data will not be written
	int unkmemberindex1; // section between vphy and vtx.?
	int numunkmember1; // only seems to be used when phy has one solid

	int unk;

	int unkindex3; // goes to the same spot as vtx normally.

	int unused1[60]; // god I hope
};

struct studiohdr_t_v16
{
	int flags;
	int checksum; // unsure if this is still checksum, there isn't any other files that have it still
	short sznameindex; // No longer stored in string block, uses string in header.
	char name[32]; // The internal name of the model, padding with null bytes.
	// Typically "my_model.mdl" will have an internal name of "my_model"
	byte unk_v16;

	byte surfacepropLookup; // saved in the file

	float mass;

	int unk1_v16;

	uint16 hitboxsetindex;
	byte numhitboxsets;

	byte illumpositionattachmentindex;

	Vector3 illumposition;	// illumination center

	Vector3 hull_min;		// ideal movement hull size
	Vector3 hull_max;

	Vector3 view_bbmin;		// clipping bounding box
	Vector3 view_bbmax;

	short numbones; // bones
	uint16 boneindex;
	uint16 bonedataindex;

	short numlocalseq; // sequences
	uint16 localseqindex;

	byte unkfill[5];

	byte numlocalattachments;
	uint16 localattachmentindex;

	short numlocalnodes;
	uint16 localnodenameindex;
	uint16 localnodeindex;

	short numikchains;
	uint16 ikchainindex;

	short numtextures; // the material limit exceeds 128, probably 256.
	uint16 textureindex;

	// replaceable textures tables
	short numskinref;
	short numskinfamilies;
	uint16 skinindex;

	short numbodyparts;
	uint16 bodypartindex;

	// this is rui meshes
	short numruimeshes;
	uint16 ruimeshindex;

	short numlocalposeparameters;
	uint16 localposeparamindex;

	uint16 surfacepropindex;

	uint16 keyvalueindex;

	uint16 vgmeshindex;
	short numvgmeshes;

	short bonetablebynameindex;

	uint16 boneremapindex;
	short numboneremaps;

	uint16 vgloddataindex;
	short numvgloddata;

	uint16 vglodheaderindex;
	short numvglodheader;

	float fadedistance;

	float gathersize; // what. from r5r struct

	short numsrcbonetransform;
	uint16 srcbonetransformindex;

	// asset bakery strings if it has any
	uint16 mayaindex;

	uint16 linearboneindex;

	short m_nBoneFlexDriverCount; // unsure if that's what it is in apex
	uint16 m_nBoneFlexDriverIndex;
	uint16 unkindexflex;

	short unkcount3; // same as v54
	uint16 unkindex3; // same as v54

	uint16 unkindex4; // same as v54

	byte unk5_v16; // unk4_v54[0]
	byte unk6_v16; // unk4_v54[1]
	short unk7_v16; // unk4_v54[2]
	short unk8_v16;
	short unk9_v16;

	//uint16 unkshorts[7];
};

struct mstudiolinearbone_t_v16
{
	unsigned short numbones;

	unsigned short flagsindex; // int

	unsigned short parentindex; // short

	unsigned short posindex; // vector3

	unsigned short quatindex; // quaternion

	unsigned short rotindex; // radianeuler

	unsigned short posetoboneindex; // matrix3x4_t
};

struct vgloddata_t_v16
{
	int vgoffset; // offset to this section in vg
	int unk;
	int vgsize; // decompressed size of data in vg

	byte numMeshes;

	// this could also be vg section index
	byte lodlevel; // 0, 1, 2, 3, etc
	byte numlods; // normally 1, stolen from vg structs

	// ids for something
	byte unk2; // powers of two
};

struct mstudiobone_t
{
	uint32_t NameOffset;		// Relative to current position
	int32_t ParentIndex;

	int BoneControllers[6]; // -1 if none

	Math::Vector3 Position;		// Local
	Math::Quaternion Rotation;	// Local

	uint8_t Padding[0x78];
};

struct r2mstudiobone_t
{
	uint32_t NameOffset;		// Relative to current position
	int32_t ParentIndex;

	int BoneControllers[6]; // -1 if none

	Math::Vector3 Position;		// Local
	Math::Quaternion Rotation;	// Local
	Math::Vector3 EulerRotation;

	Math::Vector3 PositionScale;
	Math::Vector3 RotationScale;

	uint8_t Padding[0x88];
};

struct mstudiobone_t_v16
{
	int contents;
	short unk;
	short surfacepropidx;
	short unk1;
	short sznameindex;
};

struct mstudiomodelv54_t
{
	char name[64];

	int unkindex2; // byte before string block

	// these are probably still used but get written over the name if nothing is set.
	//int type;
	//float boundingradius;

	char name2[8]; // it looks like they write the entire name
	// then write over it with other values where needed
	// why.

	int nummeshes;
	int meshindex;

	// cache purposes
	int numvertices; // number of unique vertices/normals/texcoords
	int vertexindex; // vertex Vector
	int tangentsindex; // tangents Vector

	int numattachments;
	int attachmentindex;

	// might be cut
	int numeyeballs;
	int eyeballindex;

	//mstudio_modelvertexdata_t vertexdata;

	// same as v53, except trimming the fat
	int unk[4];

	int unkindex;
	int unkindex1;
};

struct RMdlMeshStreamHeader
{
	uint32_t Version;
	uint32_t VertCacheSize;

	uint16_t MaxBonesPerStrip;
	uint16_t MaxBonesPerTri;

	uint32_t MaxBonesPerVert;
	uint32_t Hash;

	uint32_t NumLods;

	uint32_t MaterialReplacementOffset;		// Add 0x8 * 0x4 to get end of file.

	uint32_t NumBodyParts;
	uint32_t BodyPartOffset;
};

struct vertexFileHeader_t
{
	int id;
	int version;
	uint32_t checksum;

	int numLODs;
	int numLODVertexes[8];

	int numFixups;
	int fixupTableStart;

	int vertexDataStart;
	int tangentDataStart;
};

struct RMdlVGHeaderOld
{
	uint32_t Magic;		// 0x47567430	'0tvg'
	uint32_t Version;	// 0x1
	uint32_t Unknown;	// Usually 0
	uint32_t DataSize;	// Total size of data + header in starpak

	uint64_t BoneRemapOffset;
	uint64_t BoneRemapCount;		// Only 1 byte each

	uint64_t MeshOffset;
	uint64_t MeshCount;		// 0x48 each

	uint64_t IndexOffset;
	uint64_t IndexCount;		// 0x2 each (uint16_t)

	uint64_t VertexBufferOffset;
	uint64_t VertexBufferSize; // 1 byte each

	uint64_t ExtendedWeightsOffset;
	uint64_t ExtendedWeightsCount;		// Only 1 byte per count

	uint64_t Unknown2Offset;
	uint64_t Unknown2Count;		// 0x30 each

	uint64_t LodOffset;
	uint64_t LodCount;			// 0x8 each

	uint64_t ExternalWeightsOffset;
	uint64_t ExternalWeightsCount;	// 0x10 each

	uint64_t StripsOffset;
	uint64_t StripsCount;			// 0x23 each
};

struct RMdlVGHeader
{
	int id;		// 0x47567430	'0tVG'
	int version;	// 0x1
	int padding;
	uint32_t lodCount;	// If 0x1, this IS the first and only lod, if > 0x1, MORE 0tVG headers follow PER lod count
	uint32_t unk;
	uint32_t unk1;
	uint32_t lodOffset;
	char unk3[8];
};

struct VGHeader_t_v16
{
	short unk0;
	short unk2;
	int unk4;
	int nummeshes;
	int meshindex;
};

struct VGLod
{
	char unk[4];
	uint32_t dataSize;
	short meshCount;
	char unk1; // both of these bytes line up with the LOD index
	char unk2;
	float distance;
	uint64_t meshOffset;
};

struct RMdlVGIndexCountPacked
{
	uint64_t Count : 56;
	uint64_t Type : 8;
};

struct RMdlVGIndexCountPacked_V16
{
	unsigned int Count : 24;
	unsigned int Type : 8;
};

struct VGMesh_t_v16
{
	uint64_t flags;
	int vertexCount;
	short vertexSize;
	short unk;
	int indexOffset;
	RMdlVGIndexCountPacked_V16 indexPacked;
	int vertexOffset;
	int vertexBufferSize;

	int weightsOffset;
	int weightsCount;

	int unkOffset;
	int unkCount;
};

struct RMdlVGMesh
{
	uint32_t Flags1;					// Flags that pertain to this mesh
	uint32_t Flags2;					// Also flags that pertain to this mesh
	uint32_t VertexBufferStride;		// Stride in bytes of the vertex buffer
	uint32_t VertexCount;				// Count of vertices used

	uint64_t IndexOffset;
	RMdlVGIndexCountPacked IndexPacked;	// 0x2 each (uint16_t)

	uint64_t VertexOffset;
	uint64_t VertexCountBytes;		// 0x1 each aka, in bytes

	uint64_t ExtendedWeightsOffset;
	uint64_t ExtendedWeightsCount;		// Only 1 byte per count

	uint64_t ExternalWeightsOffset;
	uint64_t ExternalWeightsCount;	// 0x10 each

	uint64_t StripsOffset;
	uint64_t StripsCount;			// 0x23 each
};

struct RMdlVGMesh_V14
{
	uint32_t Flags1;					// Flags that pertain to this mesh
	uint32_t Flags2;					// Also flags that pertain to this mesh
	uint32_t VertexBufferStride;		// Stride in bytes of the vertex buffer
	uint32_t VertexCount;				// Count of vertices used

	uint64_t IndexOffset;
	RMdlVGIndexCountPacked IndexPacked;	// 0x2 each (uint16_t)

	uint64_t VertexOffset;
	uint64_t VertexCountBytes;		// 0x1 each aka, in bytes

	uint64_t ExtendedWeightsOffset;
	uint64_t ExtendedWeightsCount; // idk if these are actually unused but it looks like they are

	uint64_t ExternalWeightsOffset;
	uint64_t ExternalWeightsCount;	// 0x10 each

	uint64_t StripsOffset;
	uint64_t StripsCount;			// 0x23 each

	uint64_t UnkOffset;
	uint64_t UnkCount;		// Only 1 byte per count
};

struct RMdlVGMeshOld
{
	uint32_t Flags1;					// Flags that pertain to this mesh
	uint32_t Flags2;					// Also flags that pertain to this mesh
	uint32_t VertexOffsetBytes;			// Offset into vertex buffer by bytes
	uint32_t VertexBufferStride;		// Stride in bytes of the vertex buffer
	uint32_t VertexCount;				// Count of vertices used
	uint32_t Int6;
	uint32_t ExtendedWeightsOffset;		// Offset into the extended weights buffer
	uint32_t ExtendedWeightsSize;		// Size or count of extended weights used by this mesh
	uint32_t IndexOffset;				// Some form of index offset
	uint32_t IndexCount;				// Some form of index count
	uint32_t VertexOffset2;				// Some form of vertex offset (Not always used??)
	uint32_t VertexCount2;				// some form of vertex count
	uint32_t StripsIndex;				// Index into the strips structs
	uint32_t StripsCount;
	uint32_t Int15;
	uint32_t Int16;
	uint32_t Int17;
	uint32_t Int18;
};

struct RMdlVGStrip
{
	uint32_t IndexCount;
	uint32_t IndexOffset;

	uint32_t VertexCount;
	uint32_t VertexOffset;

	uint16_t NumBones;

	uint8_t StripFlags;

	uint8_t Padding[0x10];
};

struct RMdlVGLod
{
	uint16_t MeshIndex;
	uint16_t MeshCount;
	float Distance;
};

struct RMdlPhyHeader
{
	uint32_t HeaderSize;
	uint32_t Id;
	uint32_t SolidCount;
	uint32_t Checksum;
	uint32_t TextOffset; // offset to the text section
};

#define LAST_IND(x,part_type)    (sizeof(x)/sizeof(part_type) - 1)
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN
#  define LOW_IND(x,part_type)   LAST_IND(x,part_type)
#  define HIGH_IND(x,part_type)  0
#else
#  define HIGH_IND(x,part_type)  LAST_IND(x,part_type)
#  define LOW_IND(x,part_type)   0
#endif

#define BYTEn(x, n)   (*((uint8_t*)&(x)+n))
#define WORDn(x, n)   (*((uint16_t*)&(x)+n))
#define DWORDn(x, n)  (*((uint32_t*)&(x)+n))
#define LOBYTE(x)  BYTEn(x,LOW_IND(x,uint8_t))
#define LOWORD(x)  WORDn(x,LOW_IND(x,uint16_t))
#define LODWORD(x) DWORDn(x,LOW_IND(x,uint32_t))
#define HIBYTE(x)  BYTEn(x,HIGH_IND(x,uint8_t))
#define HIWORD(x)  WORDn(x,HIGH_IND(x,uint16_t))
#define HIDWORD(x) DWORDn(x,HIGH_IND(x,uint32_t))
#define BYTE1(x)   BYTEn(x,  1)         // byte 1 (counting from 0)
#define BYTE2(x)   BYTEn(x,  2)

struct RMdlPackedVertexPosition
{
	uint32_t _Value[2];

	Math::Vector3 Unpack()
	{
		float x, y, z;

		x = ((_Value[0] & 0x1FFFFF) * 0.0009765625) - 1024.0;
		y = ((((_Value[1] & 0x3FFu) << 11) + (_Value[0] >> 21)) * 0.0009765625) - 1024.0;
		z = ((_Value[1] >> 10) * 0.0009765625) - 2048.0;

		return Math::Vector3(x, y, z);
	}
};

struct RMdlPackedVertexTBN
{
	uint32_t _Value;

	// uint32_t _Value = 0b11011111111111111111110000110010;
	//                     ^
	//        binorm sign==|^^
	//norm dropped component||^
	//             norm sign==|^^^^^^^^^
	//                           norm1  ^^^^^^^^^
	//                                    norm2  ^^^^^^^^^^
	//                                           packedTangent

	Math::Vector3 UnpackNormal()
	{
		float x, y, z;

		float v87 = ((2 * _Value) >> 30);
		int v88 = 255;
		if (((8 * _Value) >> 31) != 0.0)
			v88 = -255;
		float v89 = (float)v88;
		float v90 = ((_Value << 13) >> 23) + -256.0;
		float v91 = ((16 * _Value) >> 23) + -256.0;
		float v92 = ((v91 * v91) + (255.0 * 255.0)) + (v90 * v90);

		float v93 = sqrtf(v92);
		int v97 = 0;

		float v1, v2, v3;

		v1 = v90 * (1.0 / v93);
		v2 = v89 * (1.0 / v93);
		v3 = v91 * (1.0 / v93);
		if (v87 == 1.0)
			v97 = -1;
		else
			v97 = 0;
		if (v87 == 2.0)
		{
			x = v3;
			y = v1;
			z = v2;
		}
		else
		{
			x = v2;
			y = v3;
			z = v1;
		}
		if (!v97)
		{
			v1 = x;
			v2 = y;
			v3 = z;
		}
		return Math::Vector3(v1, v2, v3);
	}

	Math::Vector3 UnpackTangent(Math::Vector3 Normal)
	{
		float x = (int32_t)_Value & 1023; // r2.x
		float r2y = 1 + Normal.Z;
		r2y = 1.f / r2y;  // rcp
		float r2z = -r2y * Normal.X;
		float r2w = Normal.Y * Normal.Y;
		float r3x = r2z * Normal.Y;
		float r4x = -r2y * r2w + 1;
		float r4y = -Normal.X;
		float r4z = -Normal.Y;
		float r3z = r2z * Normal.X + 1;
		float r3y;
		float r3w = r4y;
		if (Normal.Z < -0.999899983)  // r1.w
		{
			r2y = 0;
			r2z = -1;
			r2w = 0;
		}
		else
		{
			r2y = r3z;
			r2z = r3x;
			r2w = r3w;
		}
		float r4w = r3x;
		if (Normal.Z < -0.999899983)  // r1.w
		{
			r3x = -1;
			r3y = 0;
			r3z = 0;
		}
		else
		{
			r3x = r4w;
			r3y = r4x;
			r3z = r4z;
		}
		x = 0.00614192151 * x;
		float r2x = sin(x);
		r4x = cos(x);
		r3x *= r2x;
		r3y *= r2x;
		r3z *= r2x;
		r2x = r2y * r4x + r3x;
		r2y = r2z * r4x + r3y;
		r2z = r2w * r4x * r3z;

		// normalizing
		float r1w = r2x * r2x + r2y * r2y + r2z * r2z;
		r1w = 1.f / sqrt(r1w);
		r2x *= r1w;
		r2y *= r1w;
		r2z *= r1w;

		return Math::Vector3(r2x, r2y, r2z);
	}

	int8_t GetBinormalSign()
	{
		return _Value >> 31 ? -1 : 1;
	}
};

struct RMdlPackedVertexWeights
{
	uint16_t BlendWeights[2];
	uint8_t BlendIds[4];
};

struct RMdlVGExternalWeights
{
	union
	{
		struct
		{
			uint16_t Weights[4];
			uint32_t Unknown;
		};

		float SimpleWeights[3];
	};

	uint8_t WeightIds[3];
	uint8_t NumWeights;
};

struct RMdlFixup
{
	uint32_t LodIndex;

	uint32_t VertexIndex;
	uint32_t VertexCount;
};

struct RMdlVertex
{
	union
	{
		struct
		{
			uint16_t Weights[4];
			uint32_t WeightTableIndex;
		};

		float SimpleWeights[3];
	};

	uint8_t WeightIds[3];
	uint8_t NumWeights;

	Math::Vector3 Position;
	Math::Vector3 Normal;
	Math::Vector2 UVs;
};

struct mstudiobodyparts_short_t
{
	uint32_t nummodels;
	uint32_t modelindex;
};

struct mstudiobodyparts_t
{
	int sznameindex;
	int nummodels;
	int base;
	int modelindex;
};

struct mstudiobonev54_t
{
	int sznameindex;

	int parent; // parent bone
	int bonecontroller[6]; // bone controller index, -1 == none

	// default values
	Vector3 pos;
	Math::Quaternion quat;
	Math::Vector3 rot; // radianeuler

	Vector3 unkvector; // the same as whatever v53 is

	matrix3x4_t poseToBone;
	Math::Quaternion qAlignment;

	int flags;
	int proctype;
	int procindex; // procedural rule
	int physicsbone; // index into physically simulated bone

	int surfacepropidx; // index into string tablefor property name

	int contents; // See BSPFlags.h for the contents flags

	int unk; // similar value in studiohdr

	int surfacepropLookup; // unsure, the normal spot has data though

	int unkid; // id is for unk section after stringblock, lacks toggle
};

struct mstudioattachmentv54_t
{
	int sznameindex;
	int	flags;

	int	localbone; // parent bone

	matrix3x4_t	localmatrix; // attachment point
};

// $hboxset, $hbox
struct mstudiohitboxset_t
{
	int sznameindex;
	int numhitboxes;
	int hitboxindex;
};

struct mstudiobboxv54_t
{
	int bone;
	int group; // intersection group

	Vector3 bbmin; // bounding box
	Vector3 bbmax;

	int szhitboxnameindex; // offset to the name of the hitbox.

	int unk;
	int keyvalueindex; // used for KV names in string block, should be set to hitboxname if kv unneeded.
};

struct RMdlModel
{
	uint32_t NumLods;
	uint32_t LodOffset;
};

struct RMdlTitanfallModel
{
	char Name[0x40];

	int type;
	float boundingradius;

	int nummeshes;
	int meshindex;
	int numvertices;
	int vertexindex;
	int tangentsindex;

	int numattachments;
	int attachmentindex;

	// might be cut
	int numeyeballs;
	int eyeballindex;

	//mstudio_modelvertexdata_t vertexdata;

	int unk[4];

	int unkindex;
	int unkindex1;

	int unused[4];
};

struct RMdlLod
{
	uint32_t SubmeshCount;
	uint32_t SubmeshOffset;
	float Distance;
};

struct RMdlSubmesh
{
	uint32_t NumStripGroups;
	uint32_t StripGroupOffset;

	uint8_t Flags;
};

struct RMdlStripGroup
{
	uint32_t VertexCount;
	uint32_t VertexOffset;

	uint32_t IndexCount;
	uint32_t IndexOffset;

	uint32_t NumStrips;
	uint32_t StripOffset;

	uint8_t Flags;

	int numTopologyIndices;
	uint32_t topologyOffset;
};

struct RMdlStrip
{
	uint32_t IndexCount;
	uint32_t IndexOffset;

	uint32_t VertexCount;
	uint32_t VertexOffset;

	uint16_t NumBones;

	uint8_t StripFlags;

	uint32_t NumBoneChanges;
	uint32_t BoneChangeOffset;
};

struct RMdlStripVert
{
	uint8_t BoneWeightIndex[3];
	uint8_t NumBones;

	uint16_t VertexIndex;

	uint8_t BoneIds[3];
};

struct RMdlExtendedWeight
{
	uint16_t Weight;
	uint16_t BoneId;
};

struct mstudio_meshvertexdata_t
{
	int unk;
	int numLODVertexes[8];
};

struct mstudiomesh_s3_t
{
	int material;

	int modelindex;

	int numvertices; // number of unique vertices/normals/texcoords
	int vertexoffset; // vertex mstudiovertex_t

	// Access thin/fat mesh vertex data (only one will return a non-NULL result)

	int numflexes; // vertex animation
	int flexindex;

	// special codes for material operations
	int materialtype;
	int materialparam;

	// a unique ordinal for this mesh
	int meshid;

	Vector3 center;

	mstudio_meshvertexdata_t vertexdata;

	int unused[2];
};

struct mstudiomesh_v121_t
{
	int material;

	int modelindex;

	int numvertices; // number of unique vertices/normals/texcoords
	int vertexoffset; // vertex mstudiovertex_t

	// a unique ordinal for this mesh
	int meshid;

	Vector3 center;

	// a unique ordinal for this mesh
	//int meshid;

	mstudio_meshvertexdata_t vertexdata;

	int unused[2];
};

struct mstudiomesh_t_v16
{
	short material;

	// a unique ordinal for this mesh
	short meshid;

	//short modelindex;

	byte unk[4];

	//short numvertices; // number of unique vertices/normals/texcoords
	//short vertexoffset; // vertex mstudiovertex_t

	Vector3 center;
};

struct mstudiobodyparts_t_v16
{
	short sznameindex;
	uint16 modelindex;
	int base;
	int nummodels;
	int meshindex; // index into models array
};

struct RMdlTitanfallLodSubmesh
{
	uint32_t Index;

	uint16_t Unknown1;
	uint16_t Unknown2;
	uint16_t Unknown3;
	uint16_t Unknown4;

	uint8_t UnknownPad[0x28];

	uint32_t LodVertCounts[8];

	uint8_t UnknownPad2[0x20];
};

struct mstudiotexturev54_t
{
	int sznameindex;
	uint64_t guid;
};
#pragma pack(pop)

// Game helper structs
struct RMdlFixupPatches
{
	List<mstudiotexturev54_t>* Materials;
	List<uint8_t>* BoneRemaps;
	string MaterialPath;

	uint64_t VertexShift;
	uint64_t MeshOffset;
	uint64_t WeightsTableOffset;
};

struct RMdlMaterial
{
	string MaterialName;

	string AlbedoMapName;
	string NormalMapName;
	string GlossMapName;
	string SpecularMapName;
	string EmissiveMapName;
	string AmbientOcclusionMapName;
	string CavityMapName;

	uint64_t AlbedoHash;
	uint64_t NormalHash;
	uint64_t GlossHash;
	uint64_t SpecularHash;
	uint64_t EmissiveHash;
	uint64_t AmbientOcclusionHash;
	uint64_t CavityHash;
};

//ASSERT_SIZE(mstudiobone_t, 0xB4);
//ASSERT_SIZE(RMdlMeshStreamHeader, 0x24);
//ASSERT_SIZE(vertexFileHeader_t, 0x40);
//ASSERT_SIZE(RMdlFixup, 0xC);
//ASSERT_SIZE(RMdlVertex, 0x30);
//ASSERT_SIZE(mstudiobodyparts_short_t, 0x8);
//ASSERT_SIZE(RMdlModel, 0x8);
//ASSERT_SIZE(RMdlLod, 0xC);
//ASSERT_SIZE(RMdlSubmesh, 0x9);
//ASSERT_SIZE(RMdlStripGroup, 0x19);
//ASSERT_SIZE(RMdlStripVert, 0x9);
//ASSERT_SIZE(RMdlStrip, 0x1B);
//ASSERT_SIZE(RMdlExtendedWeight, 0x4);
//ASSERT_SIZE(RMdlTexture, 0xC);