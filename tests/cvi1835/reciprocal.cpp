/**
 */
#include <cvimath_internal.h>
#include <test_cvikernel_util.h>

#include <cfloat>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <string>
//#define DBG

using namespace std;

/**
 * pre_data means we test fixed pattern, it should be same sa lut
 */
enum TEST_MODE {
  PRE_DATA_COMPARE_FIX = 0,   // pre-data + fix compare
  GEN_POW_20_DATA_MAX_ERROR,  // generate 2^-20 ~ 2^20 value that check epsilon
  TEST_MODE_MAX,
};

static TEST_MODE mode;

static uint16_t test_pattern[] = {
    0x0000, 0x38D2, 0x3952, 0x399D, 0x39D2, 0x3A03, 0x3A1D, 0x3A38, 0x3A52, 0x3A6C, 0x3A83, 0x3A90,
    0x3A9D, 0x3AAA, 0x3AB8, 0x3AC5, 0x3AD2, 0x3ADF, 0x3AEC, 0x3AF9, 0x3B03, 0x3B0A, 0x3B10, 0x3B17,
    0x3B1D, 0x3B24, 0x3B2A, 0x3B31, 0x3B38, 0x3B3E, 0x3B45, 0x3B4B, 0x3B52, 0x3B58, 0x3B5F, 0x3B65,
    0x3B6C, 0x3B72, 0x3B79, 0x3B80, 0x3B83, 0x3B86, 0x3B8A, 0x3B8D, 0x3B90, 0x3B93, 0x3B97, 0x3B9A,
    0x3B9D, 0x3BA1, 0x3BA4, 0x3BA7, 0x3BAA, 0x3BAE, 0x3BB1, 0x3BB4, 0x3BB8, 0x3BBB, 0x3BBE, 0x3BC1,
    0x3BC5, 0x3BC8, 0x3BCB, 0x3BCE, 0x3BD2, 0x3BD5, 0x3BD8, 0x3BDC, 0x3BDF, 0x3BE2, 0x3BE5, 0x3BE9,
    0x3BEC, 0x3BEF, 0x3BF2, 0x3BF6, 0x3BF9, 0x3BFC, 0x3C00, 0x3C01, 0x3C03, 0x3C05, 0x3C06, 0x3C08,
    0x3C0A, 0x3C0B, 0x3C0D, 0x3C0F, 0x3C10, 0x3C12, 0x3C13, 0x3C15, 0x3C17, 0x3C18, 0x3C1A, 0x3C1C,
    0x3C1D, 0x3C1F, 0x3C21, 0x3C22, 0x3C24, 0x3C25, 0x3C27, 0x3C29, 0x3C2A, 0x3C2C, 0x3C2E, 0x3C2F,
    0x3C31, 0x3C33, 0x3C34, 0x3C36, 0x3C38, 0x3C39, 0x3C3B, 0x3C3C, 0x3C3E, 0x3C40, 0x3C41, 0x3C43,
    0x3C45, 0x3C46, 0x3C48, 0x3C4A, 0x3C4B, 0x3C4D, 0x3C4E, 0x3C50, 0x3C52, 0x3C53, 0x3C55, 0x3C57,
    0x3C58, 0x3C5A, 0x3C5C, 0x3C5D, 0x3C5F, 0x3C60, 0x3C62, 0x3C64, 0x3C65, 0x3C67, 0x3C69, 0x3C6A,
    0x3C6C, 0x3C6E, 0x3C6F, 0x3C71, 0x3C72, 0x3C74, 0x3C76, 0x3C77, 0x3C79, 0x3C7B, 0x3C7C, 0x3C7E,
    0x3C80, 0x3C81, 0x3C81, 0x3C82, 0x3C83, 0x3C84, 0x3C85, 0x3C86, 0x3C86, 0x3C87, 0x3C88, 0x3C89,
    0x3C8A, 0x3C8A, 0x3C8B, 0x3C8C, 0x3C8D, 0x3C8E, 0x3C8F, 0x3C8F, 0x3C90, 0x3C91, 0x3C92, 0x3C93,
    0x3C93, 0x3C94, 0x3C95, 0x3C96, 0x3C97, 0x3C98, 0x3C98, 0x3C99, 0x3C9A, 0x3C9B, 0x3C9C, 0x3C9C,
    0x3C9D, 0x3C9E, 0x3C9F, 0x3CA0, 0x3CA1, 0x3CA1, 0x3CA2, 0x3CA3, 0x3CA4, 0x3CA5, 0x3CA5, 0x3CA6,
    0x3CA7, 0x3CA8, 0x3CA9, 0x3CAA, 0x3CAA, 0x3CAB, 0x3CAC, 0x3CAD, 0x3CAE, 0x3CAE, 0x3CAF, 0x3CB0,
    0x3CB1, 0x3CB2, 0x3CB3, 0x3CB3, 0x3CB4, 0x3CB5, 0x3CB6, 0x3CB7, 0x3CB8, 0x3CB8, 0x3CB9, 0x3CBA,
    0x3CBB, 0x3CBC, 0x3CBC, 0x3CBD, 0x3CBE, 0x3CBF, 0x3CC0, 0x3CC1, 0x3CC1, 0x3CC2, 0x3CC3, 0x3CC4,
    0x3CC5, 0x3CC5, 0x3CC6, 0x3CC7, 0x3CC8, 0x3CC9, 0x3CCA, 0x3CCA, 0x3CCB, 0x3CCC, 0x3CCD, 0x3CCE,
    0x3CCE, 0x3CCF, 0x3CD0, 0x3CD1, 0x3CD2, 0x3CD3, 0x3CD3, 0x3CD4, 0x3CD5, 0x3CD6, 0x3CD7, 0x3CD7,
    0x3CD8, 0x3CD9, 0x3CDA, 0x3CDB, 0x3CDC, 0x3CDC, 0x3CDD, 0x3CDE, 0x3CDF, 0x3CE0, 0x3CE0, 0x3CE1,
    0x3CE2, 0x3CE3, 0x3CE4, 0x3CE5, 0x3CE5, 0x3CE6, 0x3CE7, 0x3CE8, 0x3CE9, 0x3CE9, 0x3CEA, 0x3CEB,
    0x3CEC, 0x3CED, 0x3CEE, 0x3CEE, 0x3CEF, 0x3CF0, 0x3CF1, 0x3CF2, 0x3CF2, 0x3CF3, 0x3CF4, 0x3CF5,
    0x3CF6, 0x3CF7, 0x3CF7, 0x3CF8, 0x3CF9, 0x3CFA, 0x3CFB, 0x3CFB, 0x3CFC, 0x3CFD, 0x3CFE, 0x3CFF,
    0x3D00, 0x3D00, 0x3D01, 0x3D01, 0x3D01, 0x3D02, 0x3D02, 0x3D03, 0x3D03, 0x3D03, 0x3D04, 0x3D04,
    0x3D05, 0x3D05, 0x3D06, 0x3D06, 0x3D06, 0x3D07, 0x3D07, 0x3D08, 0x3D08, 0x3D08, 0x3D09, 0x3D09,
    0x3D0A, 0x3D0A, 0x3D0A, 0x3D0B, 0x3D0B, 0x3D0C, 0x3D0C, 0x3D0C, 0x3D0D, 0x3D0D, 0x3D0E, 0x3D0E,
    0x3D0F, 0x3D0F, 0x3D0F, 0x3D10, 0x3D10, 0x3D11, 0x3D11, 0x3D11, 0x3D12, 0x3D12, 0x3D13, 0x3D13,
    0x3D13, 0x3D14, 0x3D14, 0x3D15, 0x3D15, 0x3D16, 0x3D16, 0x3D16, 0x3D17, 0x3D17, 0x3D18, 0x3D18,
    0x3D18, 0x3D19, 0x3D19, 0x3D1A, 0x3D1A, 0x3D1A, 0x3D1B, 0x3D1B, 0x3D1C, 0x3D1C, 0x3D1C, 0x3D1D,
    0x3D1D, 0x3D1E, 0x3D1E, 0x3D1F, 0x3D1F, 0x3D1F, 0x3D20, 0x3D20, 0x3D21, 0x3D21, 0x3D21, 0x3D22,
    0x3D22, 0x3D23, 0x3D23, 0x3D23, 0x3D24, 0x3D24, 0x3D25, 0x3D25, 0x3D25, 0x3D26, 0x3D26, 0x3D27,
    0x3D27, 0x3D28, 0x3D28, 0x3D28, 0x3D29, 0x3D29, 0x3D2A, 0x3D2A, 0x3D2A, 0x3D2B, 0x3D2B, 0x3D2C,
    0x3D2C, 0x3D2C, 0x3D2D, 0x3D2D, 0x3D2E, 0x3D2E, 0x3D2E, 0x3D2F, 0x3D2F, 0x3D30, 0x3D30, 0x3D31,
    0x3D31, 0x3D31, 0x3D32, 0x3D32, 0x3D33, 0x3D33, 0x3D33, 0x3D34, 0x3D34, 0x3D35, 0x3D35, 0x3D35,
    0x3D36, 0x3D36, 0x3D37, 0x3D37, 0x3D38, 0x3D38, 0x3D38, 0x3D39, 0x3D39, 0x3D3A, 0x3D3A, 0x3D3A,
    0x3D3B, 0x3D3B, 0x3D3C, 0x3D3C, 0x3D3C, 0x3D3D, 0x3D3D, 0x3D3E, 0x3D3E, 0x3D3E, 0x3D3F, 0x3D3F,
    0x3D40, 0x3D40, 0x3D41, 0x3D41, 0x3D41, 0x3D42, 0x3D42, 0x3D43, 0x3D43, 0x3D43, 0x3D44, 0x3D44,
    0x3D45, 0x3D45, 0x3D45, 0x3D46, 0x3D46, 0x3D47, 0x3D47, 0x3D47, 0x3D48, 0x3D48, 0x3D49, 0x3D49,
    0x3D4A, 0x3D4A, 0x3D4A, 0x3D4B, 0x3D4B, 0x3D4C, 0x3D4C, 0x3D4C, 0x3D4D, 0x3D4D, 0x3D4E, 0x3D4E,
    0x3D4E, 0x3D4F, 0x3D4F, 0x3D50, 0x3D50, 0x3D50, 0x3D51, 0x3D51, 0x3D52, 0x3D52, 0x3D53, 0x3D53,
    0x3D53, 0x3D54, 0x3D54, 0x3D55, 0x3D55, 0x3D55, 0x3D56, 0x3D56, 0x3D57, 0x3D57, 0x3D57, 0x3D58,
    0x3D58, 0x3D59, 0x3D59, 0x3D59, 0x3D5A, 0x3D5A, 0x3D5B, 0x3D5B, 0x3D5C, 0x3D5C, 0x3D5C, 0x3D5D,
    0x3D5D, 0x3D5E, 0x3D5E, 0x3D5E, 0x3D5F, 0x3D5F, 0x3D60, 0x3D60, 0x3D60, 0x3D61, 0x3D61, 0x3D62,
    0x3D62, 0x3D63, 0x3D63, 0x3D63, 0x3D64, 0x3D64, 0x3D65, 0x3D65, 0x3D65, 0x3D66, 0x3D66, 0x3D67,
    0x3D67, 0x3D67, 0x3D68, 0x3D68, 0x3D69, 0x3D69, 0x3D69, 0x3D6A, 0x3D6A, 0x3D6B, 0x3D6B, 0x3D6C,
    0x3D6C, 0x3D6C, 0x3D6D, 0x3D6D, 0x3D6E, 0x3D6E, 0x3D6E, 0x3D6F, 0x3D6F, 0x3D70, 0x3D70, 0x3D70,
    0x3D71, 0x3D71, 0x3D72, 0x3D72, 0x3D72, 0x3D73, 0x3D73, 0x3D74, 0x3D74, 0x3D75, 0x3D75, 0x3D75,
    0x3D76, 0x3D76, 0x3D77, 0x3D77, 0x3D77, 0x3D78, 0x3D78, 0x3D79, 0x3D79, 0x3D79, 0x3D7A, 0x3D7A,
    0x3D7B, 0x3D7B, 0x3D7B, 0x3D7C, 0x3D7C, 0x3D7D, 0x3D7D, 0x3D7E, 0x3D7E, 0x3D7E, 0x3D7F, 0x3D7F,
    0x3D80, 0x3D80, 0x3D80, 0x3D80, 0x3D81, 0x3D81, 0x3D81, 0x3D81, 0x3D81, 0x3D82, 0x3D82, 0x3D82,
    0x3D82, 0x3D82, 0x3D83, 0x3D83, 0x3D83, 0x3D83, 0x3D83, 0x3D84, 0x3D84, 0x3D84, 0x3D84, 0x3D85,
    0x3D85, 0x3D85, 0x3D85, 0x3D85, 0x3D86, 0x3D86, 0x3D86, 0x3D86, 0x3D86, 0x3D87, 0x3D87, 0x3D87,
    0x3D87, 0x3D87, 0x3D88, 0x3D88, 0x3D88, 0x3D88, 0x3D88, 0x3D89, 0x3D89, 0x3D89, 0x3D89, 0x3D89,
    0x3D8A, 0x3D8A, 0x3D8A, 0x3D8A, 0x3D8A, 0x3D8B, 0x3D8B, 0x3D8B, 0x3D8B, 0x3D8B, 0x3D8C, 0x3D8C,
    0x3D8C, 0x3D8C, 0x3D8C, 0x3D8D, 0x3D8D, 0x3D8D, 0x3D8D, 0x3D8E, 0x3D8E, 0x3D8E, 0x3D8E, 0x3D8E,
    0x3D8F, 0x3D8F, 0x3D8F, 0x3D8F, 0x3D8F, 0x3D90, 0x3D90, 0x3D90, 0x3D90, 0x3D90, 0x3D91, 0x3D91,
    0x3D91, 0x3D91, 0x3D91, 0x3D92, 0x3D92, 0x3D92, 0x3D92, 0x3D92, 0x3D93, 0x3D93, 0x3D93, 0x3D93,
    0x3D93, 0x3D94, 0x3D94, 0x3D94, 0x3D94, 0x3D94, 0x3D95, 0x3D95, 0x3D95, 0x3D95, 0x3D96, 0x3D96,
    0x3D96, 0x3D96, 0x3D96, 0x3D97, 0x3D97, 0x3D97, 0x3D97, 0x3D97, 0x3D98, 0x3D98, 0x3D98, 0x3D98,
    0x3D98, 0x3D99, 0x3D99, 0x3D99, 0x3D99, 0x3D99, 0x3D9A, 0x3D9A, 0x3D9A, 0x3D9A, 0x3D9A, 0x3D9B,
    0x3D9B, 0x3D9B, 0x3D9B, 0x3D9B, 0x3D9C, 0x3D9C, 0x3D9C, 0x3D9C, 0x3D9C, 0x3D9D, 0x3D9D, 0x3D9D,
    0x3D9D, 0x3D9D, 0x3D9E, 0x3D9E, 0x3D9E, 0x3D9E, 0x3D9F, 0x3D9F, 0x3D9F, 0x3D9F, 0x3D9F, 0x3DA0,
    0x3DA0, 0x3DA0, 0x3DA0, 0x3DA0, 0x3DA1, 0x3DA1, 0x3DA1, 0x3DA1, 0x3DA1, 0x3DA2, 0x3DA2, 0x3DA2,
    0x3DA2, 0x3DA2, 0x3DA3, 0x3DA3, 0x3DA3, 0x3DA3, 0x3DA3, 0x3DA4, 0x3DA4, 0x3DA4, 0x3DA4, 0x3DA4,
    0x3DA5, 0x3DA5, 0x3DA5, 0x3DA5, 0x3DA5, 0x3DA6, 0x3DA6, 0x3DA6, 0x3DA6, 0x3DA7, 0x3DA7, 0x3DA7,
    0x3DA7, 0x3DA7, 0x3DA8, 0x3DA8, 0x3DA8, 0x3DA8, 0x3DA8, 0x3DA9, 0x3DA9, 0x3DA9, 0x3DA9, 0x3DA9,
    0x3DAA, 0x3DAA, 0x3DAA, 0x3DAA, 0x3DAA, 0x3DAB, 0x3DAB, 0x3DAB, 0x3DAB, 0x3DAB, 0x3DAC, 0x3DAC,
    0x3DAC, 0x3DAC, 0x3DAC, 0x3DAD, 0x3DAD, 0x3DAD, 0x3DAD, 0x3DAD, 0x3DAE, 0x3DAE, 0x3DAE, 0x3DAE,
    0x3DAE, 0x3DAF, 0x3DAF, 0x3DAF, 0x3DAF, 0x3DB0, 0x3DB0, 0x3DB0, 0x3DB0, 0x3DB0, 0x3DB1, 0x3DB1,
    0x3DB1, 0x3DB1, 0x3DB1, 0x3DB2, 0x3DB2, 0x3DB2, 0x3DB2, 0x3DB2, 0x3DB3, 0x3DB3, 0x3DB3, 0x3DB3,
    0x3DB3, 0x3DB4, 0x3DB4, 0x3DB4, 0x3DB4, 0x3DB4, 0x3DB5, 0x3DB5, 0x3DB5, 0x3DB5, 0x3DB5, 0x3DB6,
    0x3DB6, 0x3DB6, 0x3DB6, 0x3DB6, 0x3DB7, 0x3DB7, 0x3DB7, 0x3DB7, 0x3DB8, 0x3DB8, 0x3DB8, 0x3DB8,
    0x3DB8, 0x3DB9, 0x3DB9, 0x3DB9, 0x3DB9, 0x3DB9, 0x3DBA, 0x3DBA, 0x3DBA, 0x3DBA, 0x3DBA, 0x3DBB,
    0x3DBB, 0x3DBB, 0x3DBB, 0x3DBB, 0x3DBC, 0x3DBC, 0x3DBC, 0x3DBC, 0x3DBC, 0x3DBD, 0x3DBD, 0x3DBD,
    0x3DBD, 0x3DBD, 0x3DBE, 0x3DBE, 0x3DBE, 0x3DBE, 0x3DBE, 0x3DBF, 0x3DBF, 0x3DBF, 0x3DBF, 0x3DBF,
    0x3DC0, 0x3DC0, 0x3DC0, 0x3DC0, 0x3DC1, 0x3DC1, 0x3DC1, 0x3DC1, 0x3DC1, 0x3DC2, 0x3DC2, 0x3DC2,
    0x3DC2, 0x3DC2, 0x3DC3, 0x3DC3, 0x3DC3, 0x3DC3, 0x3DC3, 0x3DC4, 0x3DC4, 0x3DC4, 0x3DC4, 0x3DC4,
    0x3DC5, 0x3DC5, 0x3DC5, 0x3DC5, 0x3DC5, 0x3DC6, 0x3DC6, 0x3DC6, 0x3DC6, 0x3DC6, 0x3DC7, 0x3DC7,
    0x3DC7, 0x3DC7, 0x3DC7, 0x3DC8, 0x3DC8, 0x3DC8, 0x3DC8, 0x3DC8, 0x3DC9, 0x3DC9, 0x3DC9, 0x3DC9,
    0x3DCA, 0x3DCA, 0x3DCA, 0x3DCA, 0x3DCA, 0x3DCB, 0x3DCB, 0x3DCB, 0x3DCB, 0x3DCB, 0x3DCC, 0x3DCC,
    0x3DCC, 0x3DCC, 0x3DCC, 0x3DCD, 0x3DCE, 0x3DCF, 0x3DD0, 0x3DD1, 0x3DD2, 0x3DD3, 0x3DD4, 0x3DD5,
    0x3DD6, 0x3DD7, 0x3DD8, 0x3DD9, 0x3DDA, 0x3DDB, 0x3DDC, 0x3DDD, 0x3DDE, 0x3DDF, 0x3DE0, 0x3DE1,
    0x3DE2, 0x3DE3, 0x3DE4, 0x3DE5,
};

static uint16_t test_pattern_ref[] = {
    0x7f7f, 0x461c, 0x459c, 0x4551, 0x451c, 0x44fa, 0x44d1, 0x44b2, 0x449c, 0x448b, 0x447a, 0x4464,
    0x4451, 0x4441, 0x4432, 0x4426, 0x441c, 0x4413, 0x440b, 0x4404, 0x43fa, 0x43ed, 0x43e4, 0x43d9,
    0x43d1, 0x43c8, 0x43c1, 0x43b9, 0x43b2, 0x43ac, 0x43a6, 0x43a1, 0x439c, 0x4398, 0x4393, 0x438f,
    0x438b, 0x4387, 0x4384, 0x4380, 0x437a, 0x4375, 0x436d, 0x4368, 0x4364, 0x435f, 0x4359, 0x4355,
    0x4351, 0x434c, 0x4348, 0x4344, 0x4341, 0x433c, 0x4339, 0x4336, 0x4332, 0x432f, 0x432c, 0x432a,
    0x4326, 0x4324, 0x4321, 0x431f, 0x431c, 0x431a, 0x4318, 0x4315, 0x4313, 0x4311, 0x430f, 0x430d,
    0x430b, 0x4309, 0x4307, 0x4305, 0x4304, 0x4302, 0x4300, 0x42fe, 0x42fa, 0x42f6, 0x42f5, 0x42f1,
    0x42ed, 0x42ec, 0x42e8, 0x42e5, 0x42e4, 0x42e0, 0x42df, 0x42dc, 0x42d9, 0x42d8, 0x42d5, 0x42d2,
    0x42d1, 0x42ce, 0x42cc, 0x42ca, 0x42c8, 0x42c7, 0x42c4, 0x42c2, 0x42c1, 0x42bf, 0x42bc, 0x42bb,
    0x42b9, 0x42b7, 0x42b6, 0x42b4, 0x42b2, 0x42b1, 0x42af, 0x42ae, 0x42ac, 0x42ab, 0x42aa, 0x42a8,
    0x42a6, 0x42a5, 0x42a4, 0x42a2, 0x42a1, 0x42a0, 0x429f, 0x429e, 0x429c, 0x429b, 0x429a, 0x4298,
    0x4298, 0x4296, 0x4295, 0x4294, 0x4293, 0x4292, 0x4291, 0x4290, 0x428f, 0x428e, 0x428d, 0x428c,
    0x428b, 0x428a, 0x4289, 0x4288, 0x4287, 0x4286, 0x4285, 0x4285, 0x4284, 0x4283, 0x4282, 0x4281,
    0x4280, 0x427e, 0x427e, 0x427c, 0x427a, 0x4278, 0x4276, 0x4275, 0x4275, 0x4273, 0x4271, 0x426f,
    0x426d, 0x426d, 0x426c, 0x426a, 0x4268, 0x4267, 0x4265, 0x4265, 0x4264, 0x4262, 0x4260, 0x425f,
    0x425f, 0x425d, 0x425c, 0x425a, 0x4259, 0x4258, 0x4258, 0x4256, 0x4255, 0x4253, 0x4252, 0x4252,
    0x4251, 0x424f, 0x424e, 0x424d, 0x424c, 0x424c, 0x424a, 0x4249, 0x4248, 0x4247, 0x4247, 0x4245,
    0x4244, 0x4243, 0x4242, 0x4241, 0x4241, 0x4240, 0x423f, 0x423d, 0x423c, 0x423c, 0x423b, 0x423a,
    0x4239, 0x4238, 0x4237, 0x4237, 0x4236, 0x4235, 0x4234, 0x4233, 0x4232, 0x4232, 0x4231, 0x4230,
    0x422f, 0x422e, 0x422e, 0x422d, 0x422c, 0x422c, 0x422b, 0x422a, 0x422a, 0x4229, 0x4228, 0x4227,
    0x4226, 0x4226, 0x4225, 0x4225, 0x4224, 0x4223, 0x4222, 0x4222, 0x4221, 0x4221, 0x4220, 0x421f,
    0x421f, 0x421e, 0x421e, 0x421d, 0x421c, 0x421b, 0x421b, 0x421b, 0x421a, 0x4219, 0x4218, 0x4218,
    0x4218, 0x4217, 0x4216, 0x4216, 0x4215, 0x4215, 0x4214, 0x4214, 0x4213, 0x4212, 0x4212, 0x4212,
    0x4211, 0x4210, 0x4210, 0x420f, 0x420f, 0x420e, 0x420e, 0x420d, 0x420d, 0x420d, 0x420c, 0x420b,
    0x420b, 0x420a, 0x420a, 0x420a, 0x4209, 0x4209, 0x4208, 0x4207, 0x4207, 0x4207, 0x4206, 0x4206,
    0x4205, 0x4205, 0x4205, 0x4204, 0x4204, 0x4203, 0x4203, 0x4203, 0x4202, 0x4202, 0x4201, 0x4201,
    0x4200, 0x4200, 0x41fe, 0x41fe, 0x41fe, 0x41fc, 0x41fc, 0x41fa, 0x41fa, 0x41fa, 0x41f8, 0x41f8,
    0x41f6, 0x41f6, 0x41f5, 0x41f5, 0x41f5, 0x41f3, 0x41f3, 0x41f1, 0x41f1, 0x41f1, 0x41ef, 0x41ef,
    0x41ed, 0x41ed, 0x41ed, 0x41ec, 0x41ec, 0x41ea, 0x41ea, 0x41ea, 0x41e8, 0x41e8, 0x41e7, 0x41e7,
    0x41e5, 0x41e5, 0x41e5, 0x41e4, 0x41e4, 0x41e2, 0x41e2, 0x41e2, 0x41e0, 0x41e0, 0x41df, 0x41df,
    0x41df, 0x41dd, 0x41dd, 0x41dc, 0x41dc, 0x41da, 0x41da, 0x41da, 0x41d9, 0x41d9, 0x41d8, 0x41d8,
    0x41d8, 0x41d6, 0x41d6, 0x41d5, 0x41d5, 0x41d5, 0x41d3, 0x41d3, 0x41d2, 0x41d2, 0x41d2, 0x41d1,
    0x41d1, 0x41cf, 0x41cf, 0x41ce, 0x41ce, 0x41ce, 0x41cd, 0x41cd, 0x41cc, 0x41cc, 0x41cc, 0x41ca,
    0x41ca, 0x41c9, 0x41c9, 0x41c9, 0x41c8, 0x41c8, 0x41c7, 0x41c7, 0x41c7, 0x41c5, 0x41c5, 0x41c4,
    0x41c4, 0x41c3, 0x41c3, 0x41c3, 0x41c2, 0x41c2, 0x41c1, 0x41c1, 0x41c1, 0x41c0, 0x41c0, 0x41bf,
    0x41bf, 0x41bf, 0x41bd, 0x41bd, 0x41bc, 0x41bc, 0x41bc, 0x41bb, 0x41bb, 0x41ba, 0x41ba, 0x41b9,
    0x41b9, 0x41b9, 0x41b8, 0x41b8, 0x41b7, 0x41b7, 0x41b7, 0x41b6, 0x41b6, 0x41b5, 0x41b5, 0x41b5,
    0x41b4, 0x41b4, 0x41b3, 0x41b3, 0x41b2, 0x41b2, 0x41b2, 0x41b1, 0x41b1, 0x41b0, 0x41b0, 0x41b0,
    0x41af, 0x41af, 0x41ae, 0x41ae, 0x41ae, 0x41ad, 0x41ad, 0x41ac, 0x41ac, 0x41ac, 0x41ac, 0x41ac,
    0x41ab, 0x41ab, 0x41aa, 0x41aa, 0x41aa, 0x41a9, 0x41a9, 0x41a8, 0x41a8, 0x41a8, 0x41a7, 0x41a7,
    0x41a6, 0x41a6, 0x41a6, 0x41a5, 0x41a5, 0x41a5, 0x41a5, 0x41a5, 0x41a4, 0x41a4, 0x41a3, 0x41a3,
    0x41a2, 0x41a2, 0x41a2, 0x41a1, 0x41a1, 0x41a1, 0x41a1, 0x41a1, 0x41a0, 0x41a0, 0x419f, 0x419f,
    0x419f, 0x419e, 0x419e, 0x419e, 0x419e, 0x419e, 0x419d, 0x419d, 0x419c, 0x419c, 0x419b, 0x419b,
    0x419b, 0x419b, 0x419b, 0x419a, 0x419a, 0x419a, 0x4199, 0x4199, 0x4198, 0x4198, 0x4198, 0x4198,
    0x4198, 0x4197, 0x4197, 0x4197, 0x4196, 0x4196, 0x4196, 0x4196, 0x4195, 0x4195, 0x4195, 0x4194,
    0x4194, 0x4194, 0x4194, 0x4194, 0x4193, 0x4193, 0x4192, 0x4192, 0x4192, 0x4192, 0x4192, 0x4191,
    0x4191, 0x4190, 0x4190, 0x4190, 0x4190, 0x4190, 0x418f, 0x418f, 0x418f, 0x418e, 0x418e, 0x418e,
    0x418e, 0x418e, 0x418d, 0x418d, 0x418d, 0x418d, 0x418d, 0x418c, 0x418c, 0x418b, 0x418b, 0x418b,
    0x418b, 0x418b, 0x418a, 0x418a, 0x418a, 0x418a, 0x418a, 0x4189, 0x4189, 0x4189, 0x4189, 0x4189,
    0x4188, 0x4188, 0x4187, 0x4187, 0x4187, 0x4187, 0x4187, 0x4186, 0x4186, 0x4186, 0x4186, 0x4186,
    0x4185, 0x4185, 0x4185, 0x4185, 0x4185, 0x4184, 0x4184, 0x4184, 0x4184, 0x4184, 0x4183, 0x4183,
    0x4183, 0x4183, 0x4183, 0x4182, 0x4182, 0x4182, 0x4182, 0x4181, 0x4181, 0x4181, 0x4181, 0x4181,
    0x4180, 0x4180, 0x4180, 0x4180, 0x417e, 0x417e, 0x417e, 0x417e, 0x417e, 0x417c, 0x417c, 0x417c,
    0x417c, 0x417c, 0x417a, 0x417a, 0x417a, 0x417a, 0x417a, 0x4178, 0x4178, 0x4178, 0x4178, 0x4176,
    0x4176, 0x4176, 0x4176, 0x4176, 0x4175, 0x4175, 0x4175, 0x4175, 0x4175, 0x4173, 0x4173, 0x4173,
    0x4173, 0x4173, 0x4171, 0x4171, 0x4171, 0x4171, 0x4171, 0x416f, 0x416f, 0x416f, 0x416f, 0x416f,
    0x416d, 0x416d, 0x416d, 0x416d, 0x416d, 0x416c, 0x416c, 0x416c, 0x416c, 0x416c, 0x416a, 0x416a,
    0x416a, 0x416a, 0x416a, 0x4168, 0x4168, 0x4168, 0x4168, 0x4167, 0x4167, 0x4167, 0x4167, 0x4167,
    0x4165, 0x4165, 0x4165, 0x4165, 0x4165, 0x4164, 0x4164, 0x4164, 0x4164, 0x4164, 0x4162, 0x4162,
    0x4162, 0x4162, 0x4162, 0x4160, 0x4160, 0x4160, 0x4160, 0x4160, 0x415f, 0x415f, 0x415f, 0x415f,
    0x415f, 0x415d, 0x415d, 0x415d, 0x415d, 0x415d, 0x415c, 0x415c, 0x415c, 0x415c, 0x415a, 0x415a,
    0x415a, 0x415a, 0x415a, 0x4159, 0x4159, 0x4159, 0x4159, 0x4159, 0x4158, 0x4158, 0x4158, 0x4158,
    0x4158, 0x4156, 0x4156, 0x4156, 0x4156, 0x4156, 0x4155, 0x4155, 0x4155, 0x4155, 0x4155, 0x4153,
    0x4153, 0x4153, 0x4153, 0x4153, 0x4152, 0x4152, 0x4152, 0x4152, 0x4152, 0x4151, 0x4151, 0x4151,
    0x4151, 0x4151, 0x414f, 0x414f, 0x414f, 0x414f, 0x414e, 0x414e, 0x414e, 0x414e, 0x414e, 0x414d,
    0x414d, 0x414d, 0x414d, 0x414d, 0x414c, 0x414c, 0x414c, 0x414c, 0x414c, 0x414a, 0x414a, 0x414a,
    0x414a, 0x414a, 0x4149, 0x4149, 0x4149, 0x4149, 0x4149, 0x4148, 0x4148, 0x4148, 0x4148, 0x4148,
    0x4147, 0x4147, 0x4147, 0x4147, 0x4147, 0x4145, 0x4145, 0x4145, 0x4145, 0x4144, 0x4144, 0x4144,
    0x4144, 0x4144, 0x4143, 0x4143, 0x4143, 0x4143, 0x4143, 0x4142, 0x4142, 0x4142, 0x4142, 0x4142,
    0x4141, 0x4141, 0x4141, 0x4141, 0x4141, 0x4140, 0x4140, 0x4140, 0x4140, 0x4140, 0x413f, 0x413f,
    0x413f, 0x413f, 0x413f, 0x413d, 0x413d, 0x413d, 0x413d, 0x413d, 0x413c, 0x413c, 0x413c, 0x413c,
    0x413c, 0x413b, 0x413b, 0x413b, 0x413b, 0x413a, 0x413a, 0x413a, 0x413a, 0x413a, 0x4139, 0x4139,
    0x4139, 0x4139, 0x4139, 0x4138, 0x4138, 0x4138, 0x4138, 0x4138, 0x4137, 0x4137, 0x4137, 0x4137,
    0x4137, 0x4136, 0x4136, 0x4136, 0x4136, 0x4136, 0x4135, 0x4135, 0x4135, 0x4135, 0x4135, 0x4134,
    0x4134, 0x4134, 0x4134, 0x4134, 0x4133, 0x4133, 0x4133, 0x4133, 0x4132, 0x4132, 0x4132, 0x4132,
    0x4132, 0x4131, 0x4131, 0x4131, 0x4131, 0x4131, 0x4130, 0x4130, 0x4130, 0x4130, 0x4130, 0x412f,
    0x412f, 0x412f, 0x412f, 0x412f, 0x412e, 0x412e, 0x412e, 0x412e, 0x412e, 0x412d, 0x412d, 0x412d,
    0x412d, 0x412d, 0x412c, 0x412c, 0x412c, 0x412c, 0x412c, 0x412c, 0x412c, 0x412c, 0x412c, 0x412c,
    0x412b, 0x412b, 0x412b, 0x412b, 0x412a, 0x412a, 0x412a, 0x412a, 0x412a, 0x4129, 0x4129, 0x4129,
    0x4129, 0x4129, 0x4128, 0x4128, 0x4128, 0x4128, 0x4128, 0x4127, 0x4127, 0x4127, 0x4127, 0x4127,
    0x4126, 0x4126, 0x4126, 0x4126, 0x4126, 0x4125, 0x4125, 0x4125, 0x4125, 0x4125, 0x4125, 0x4125,
    0x4125, 0x4125, 0x4125, 0x4124, 0x4124, 0x4124, 0x4124, 0x4124, 0x4123, 0x4123, 0x4123, 0x4123,
    0x4122, 0x4122, 0x4122, 0x4122, 0x4122, 0x4121, 0x4121, 0x4121, 0x4121, 0x4121, 0x4121, 0x4121,
    0x4121, 0x4121, 0x4121, 0x4120, 0x411f, 0x411e, 0x411e, 0x411d, 0x411c, 0x411b, 0x411b, 0x411a,
    0x4119, 0x4118, 0x4118, 0x4117, 0x4116, 0x4116, 0x4115, 0x4114, 0x4114, 0x4113, 0x4112, 0x4112,
    0x4111, 0x4110, 0x4110, 0x410f,
};

static void tl_lut_ref(uint16_t *ofmap, uint16_t *ifmap, cvk_tl_shape_t ifmap_shape) {
  for (uint32_t i = 0; i < tl_shape_size(&ifmap_shape); i++) {
    if (mode == PRE_DATA_COMPARE_FIX) {
      ofmap[i] = test_pattern_ref[i];
    } else {
      uint16_t v = convert_fp32_bf16(1 / (1.0 * (convert_bf16_fp32(ifmap[i]))));
      ofmap[i] = v;
    }
  }
}

static bool verify(uint16_t *ofmap_data, uint16_t *ref_data, uint16_t *ifmap,
                   uint64_t ifmap_shape_size, TEST_MODE mode) {
  uint64_t size = ifmap_shape_size;

  for (uint64_t i = 0; i < size; i++) {
    bool is_close;
    uint16_t ref;
    uint16_t ofmap_data_bf16;
    float ref_f;
    float ofmap_data_f;

    ref = ref_data[i];
    ref_f = convert_bf16_fp32(ref);
    ofmap_data_f = convert_bf16_fp32(ofmap_data[i]);
    ofmap_data_bf16 = ofmap_data[i];

    if (mode == PRE_DATA_COMPARE_FIX) {
      is_close = ofmap_data[i] == ref;
    } else {
      is_close = fabs(ref_f - ofmap_data_f) < 0.001;
    }

    if (!is_close) {
      fprintf(stderr,
              "comparing failed at ofmap_data[%lu](input:%e), got %x, exp %x, "
              "fp32: got %e exp %e\n",
              i, convert_bf16_fp32(ifmap[i]), ofmap_data_bf16, ref, ofmap_data_f, ref_f);
      exit(-1);
    }
  }

  return true;
}

static void gen_input(uint16_t *ifmap, uint64_t ifmap_shape_size) {
  if (mode == PRE_DATA_COMPARE_FIX) {
    memcpy(ifmap, &test_pattern, sizeof(test_pattern));
  } else {
    for (uint64_t i = 0; i < ifmap_shape_size; i++) {
      srand(static_cast<unsigned>(time(0)));
      std::random_device rd;
      std::mt19937 e2(rd());
      float LO = pow(2, -10);
      float HI = pow(2, 10);
      // std::uniform_real_distribution<> dist(pow(2,-62), pow(2,63));
      for (uint64_t i = 0; i < ifmap_shape_size; i++) {
        // float r3 = dist(e2);
        float r3 = LO + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (HI - LO)));
        ifmap[i] = convert_fp32_bf16(r3);
      }
    }
  }

#ifdef DBG
  for (uint64_t i = 0; i < ifmap_shape_size; i++) {
    printf("source if[%lu] bf16 %f 0x%x, log2f is %f\n", i, convert_bf16_fp32(ifmap[i]), ifmap[i],
           floor(log2((convert_bf16_fp32(ifmap[i])))));
  }
#endif /* ifdef DBG */
}

static void testbench(CVI_RT_HANDLE *ctx, cvk_context_t *bmk, uint32_t input_n, uint32_t input_c,
                      uint32_t input_h, uint32_t input_w) {
  cvk_fmt_t fmt = CVK_FMT_BF16;

  // TODO: check more shape / align
  cvk_tl_shape_t ifmap_shape = {input_n, input_c, input_h, input_w};
  cvk_tl_shape_t ofmap_shape = ifmap_shape;
  cvk_tl_shape_t table_shape;
  cvm_table_shape(bmk, &table_shape);

  uint64_t ifmap_shape_size = tl_shape_size(&ifmap_shape);
  uint64_t ofmap_size = tl_shape_size(&ofmap_shape);
  uint64_t table_size = tl_shape_size(&table_shape);

  // prepare input data with size
  int data_type_size = bytesize_of_fmt(fmt);
  uint64_t ifmap_bytesize = ifmap_shape_size * data_type_size;
  uint64_t ofmap_bytesize = ofmap_size * data_type_size;
  uint64_t table_bytesize = table_size * data_type_size;

  uint16_t *ifmap = (uint16_t *)xmalloc(ifmap_bytesize);
  uint16_t *ref_data = (uint16_t *)xmalloc(ofmap_bytesize);
  uint16_t *table_data = (uint16_t *)xmalloc(table_bytesize);
  uint16_t *table_data_mantissa = (uint16_t *)xmalloc(table_bytesize);

  // alloc lmem
  cvk_tl_t *tl_ifmap = test_alloc_tl(bmk, ifmap_shape, fmt, /*align*/ 1);
  cvk_tl_t *tl_ofmap_bf16 = test_alloc_tl(bmk, ofmap_shape, fmt, /*align*/ 1);
  cvk_tl_t *tl_buf = test_alloc_tl(bmk, tl_ifmap->shape, fmt, /*align*/ 1);
  cvk_tl_t *cvk_tl_table_answer = test_alloc_tl(bmk, table_shape, fmt, /*align*/ 1);
  cvk_tl_t *cvk_tl_table_answer_mantissa = test_alloc_tl(bmk, table_shape, fmt, /*align*/ 1);

  // generate testbench
  gen_input(ifmap, ifmap_shape_size);
  tl_lut_ref(ref_data, ifmap, ifmap_shape);

  // prepare table
  cvm_reciprocal_tbl(table_data, table_data_mantissa, &table_shape);

  // sys->lmem
  test_put_tensor_g2l_comp(ctx, bmk, tl_ifmap, (uint8_t *)ifmap);
  test_put_tensor_g2l_comp(ctx, bmk, cvk_tl_table_answer, (uint8_t *)table_data);
  test_put_tensor_g2l_comp(ctx, bmk, cvk_tl_table_answer_mantissa, (uint8_t *)table_data_mantissa);

  cvm_emit_reciprocal(bmk, tl_ifmap, tl_buf, cvk_tl_table_answer, cvk_tl_table_answer_mantissa,
                      tl_ofmap_bf16);

  // issue cmd
  test_submit_comp(ctx, bmk);

  // get output from lmem->sys
  uint16_t *ofmap_data = (uint16_t *)test_get_tensor_l2g_comp(ctx, bmk, tl_ofmap_bf16);

  verify(ofmap_data, ref_data, ifmap, ifmap_shape_size, mode);

  free_tl(bmk, cvk_tl_table_answer_mantissa);
  free_tl(bmk, cvk_tl_table_answer);
  free_tl(bmk, tl_buf);
  free_tl(bmk, tl_ofmap_bf16);
  free_tl(bmk, tl_ifmap);

  free(ifmap);
  free(ref_data);
  free(ofmap_data);
  free(table_data);
  free(table_data_mantissa);
}

int main() {
  CVI_RT_HANDLE ctx;
  cvk_context_t *bmk;
  int round_mode;

  round_mode = set_store_feround();

  test_init(&ctx, &bmk);

  for (int i = GEN_POW_20_DATA_MAX_ERROR; i < TEST_MODE_MAX; i++) {
    mode = static_cast<TEST_MODE>(i);
    printf("test mode %d...\n", mode);

    int input_n = 1;
    int input_c = 32;
    int input_h = 1;
    int input_w = 1;

    if (mode == PRE_DATA_COMPARE_FIX) {
      input_h = 4;
      input_w = 8;
    } else {
      input_h = input_w = 16;
    }

    testbench(&ctx, bmk, input_n, input_c, input_h, input_w);
  }

  test_exit(&ctx, bmk);
  restore_feround(round_mode);
  return 0;
}
