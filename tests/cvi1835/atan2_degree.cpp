/**
 * \breif atan2 is implemented by atan, you can refer
 * [wiki](https://en.wikipedia.org/wiki/Atan2) for more details
 */

#include <cvimath_internal.h>
#include <test_cvikernel_util.h>

#define OUT
#define IN
#include <cfloat>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <string>
//#define DBG

/**
 * pre_data means we test fixed pattern, it should be same sa lut
 */
enum TEST_MODE {
  PRE_DATA_COMPARE_FIX = 0,  // pre-data + fix compare
  DATA_COMPARE_ACCURACY,     // generate \range_start to \range_end value that
                             // check epsilon, default set x > 0, y > 0

  DATA_COMPARE_ACCURACY_X_GT_0,         // atan(y/x), x > 0, y = 0
  DATA_COMPARE_ACCURACY_X_LT_0_Y_GE_0,  // atan(y/x) + PI , x < 0 and y >= 0
  DATA_COMPARE_ACCURACY_X_LT_0_Y_LT_0,  // atan(y/x) - PI , x < 0 and y < 0
  DATA_COMPARE_ACCURACY_X_0_Y_GT_0,     // pi / 2, x = 0 and y > 0
  DATA_COMPARE_ACCURACY_X_0_Y_LT_0,     // -pi / 2, x = 0 and y < 0
  DATA_COMPARE_U8,                      // generate \range_start to \range_end value that check
                                        // epsilon, result bf16->uint8_t
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

static uint16_t golden_bf16[] = {
    0x42b4, 0x42b4, 0x42b4, 0x42b4, 0x42b4, 0x42b4, 0x42b3, 0x42b3, 0x42b3, 0x42b3, 0x42b3, 0x42b3,
    0x42b3, 0x42b3, 0x42b3, 0x42b3, 0x42b2, 0x42b2, 0x42b2, 0x42b2, 0x42b2, 0x42b2, 0x42b2, 0x42b2,
    0x42b2, 0x42b1, 0x42b1, 0x42b1, 0x42b1, 0x42b1, 0x42b1, 0x42b1, 0x42b1, 0x42b1, 0x42b1, 0x42af,
    0x42af, 0x42af, 0x42af, 0x42af, 0x42af, 0x42af, 0x42af, 0x42af, 0x42af, 0x42ae, 0x42ae, 0x42ae,
    0x42ae, 0x42ae, 0x42ae, 0x42ae, 0x42ae, 0x42ae, 0x42ad, 0x42ad, 0x42ad, 0x42ad, 0x42ad, 0x42ad,
    0x42ad, 0x42ad, 0x42ad, 0x42ac, 0x42ac, 0x42ac, 0x42ac, 0x42ac, 0x42ac, 0x42ac, 0x42ac, 0x42ac,
    0x42ab, 0x42ab, 0x42ab, 0x42ab, 0x42ab, 0x42ab, 0x42ab, 0x42ab, 0x42ab, 0x42aa, 0x42aa, 0x42aa,
    0x42aa, 0x42aa, 0x42aa, 0x42aa, 0x42aa, 0x42a9, 0x42a9, 0x42a9, 0x42a9, 0x42a9, 0x42a9, 0x42a9,
    0x42a9, 0x42a9, 0x42a7, 0x42a7, 0x42a7, 0x42a7, 0x42a7, 0x42a7, 0x42a7, 0x42a7, 0x42a6, 0x42a6,
    0x42a6, 0x42a6, 0x42a6, 0x42a6, 0x42a6, 0x42a6, 0x42a5, 0x42a5, 0x42a5, 0x42a5, 0x42a5, 0x42a5,
    0x42a5, 0x42a4, 0x42a4, 0x42a4, 0x42a4, 0x42a4, 0x42a4, 0x42a4, 0x42a4, 0x42a4, 0x42a3, 0x42a3,
    0x42a3, 0x42a3, 0x42a3, 0x42a3, 0x42a3, 0x42a2, 0x42a2, 0x42a2, 0x42a2, 0x42a2, 0x42a2, 0x42a2,
    0x42a2, 0x42a2, 0x42a1, 0x42a1, 0x42a1, 0x42a1, 0x42a1, 0x42a1, 0x42a1, 0x42a0, 0x42a0, 0x42a0,
    0x42a0, 0x42a0, 0x42a0, 0x42a0, 0x42a0, 0x429e, 0x429e, 0x429e, 0x429e, 0x429e, 0x429e, 0x429d,
    0x429d, 0x429d, 0x429d, 0x429d, 0x429d, 0x429d, 0x429d, 0x429d, 0x429c, 0x429c, 0x429c, 0x429c,
    0x429c, 0x429b, 0x429b, 0x429b, 0x429b, 0x429b, 0x429b, 0x429b, 0x429b, 0x429a, 0x429a, 0x429a,
    0x429a, 0x429a, 0x429a, 0x4299, 0x4299, 0x4299, 0x4299, 0x4299, 0x4299, 0x4299, 0x4299, 0x4298,
    0x4298, 0x4298, 0x4298, 0x4298, 0x4298, 0x4297, 0x4297, 0x4297, 0x4297, 0x4297, 0x4297, 0x4296,
    0x4296, 0x4296, 0x4296, 0x4296, 0x4295, 0x4295, 0x4295, 0x4295, 0x4295, 0x4295, 0x4295, 0x4295,
    0x4294, 0x4294, 0x4294, 0x4294, 0x4294, 0x4294, 0x4293, 0x4293, 0x4293, 0x4293, 0x4293, 0x4293,
    0x4292, 0x4292, 0x4292, 0x4292, 0x4292, 0x4291, 0x4291, 0x4291, 0x4291, 0x4291, 0x4291, 0x4291,
    0x4291, 0x428f, 0x428f, 0x428f, 0x428e, 0x428e, 0x428e, 0x428e, 0x428e, 0x428e, 0x428e, 0x428e,
    0x428d, 0x428d, 0x428d, 0x428d, 0x428c, 0x428c, 0x428c, 0x428c, 0x428c, 0x428c, 0x428c, 0x428c,
    0x428b, 0x428b, 0x428b, 0x428a, 0x428a, 0x428a, 0x428a, 0x428a, 0x428a, 0x428a, 0x4289, 0x4289,
    0x4289, 0x4288, 0x4288, 0x4288, 0x4288, 0x4288, 0x4288, 0x4287, 0x4287, 0x4287, 0x4287, 0x4287,
    0x4286, 0x4286, 0x4286, 0x4286, 0x4286, 0x4286, 0x4286, 0x4286, 0x4285, 0x4285, 0x4285, 0x4285,
    0x4285, 0x4285, 0x4285, 0x4285, 0x4285, 0x4284, 0x4284, 0x4284, 0x4284, 0x4284, 0x4283, 0x4283,
    0x4282, 0x4282, 0x4282, 0x4282, 0x4282, 0x4281, 0x4281, 0x4281, 0x4281, 0x4281, 0x4281, 0x4281,
    0x4280, 0x4280, 0x4280, 0x427e, 0x427e, 0x427e, 0x427e, 0x427e, 0x427c, 0x427c, 0x427c, 0x427a,
    0x427a, 0x427a, 0x427a, 0x427a, 0x427a, 0x4278, 0x4278, 0x4278, 0x4277, 0x4277, 0x4277, 0x4277,
    0x4277, 0x4277, 0x4275, 0x4275, 0x4275, 0x4273, 0x4273, 0x4273, 0x4273, 0x4273, 0x4271, 0x4271,
    0x4271, 0x4270, 0x4270, 0x4270, 0x4270, 0x4270, 0x4270, 0x4270, 0x426e, 0x426c, 0x426c, 0x426c,
    0x426c, 0x426c, 0x426a, 0x426a, 0x426a, 0x426a, 0x4269, 0x4269, 0x4269, 0x4269, 0x4269, 0x4267,
    0x4267, 0x4266, 0x4266, 0x4266, 0x4266, 0x4266, 0x4264, 0x4264, 0x4264, 0x4262, 0x4262, 0x4262,
    0x4262, 0x4261, 0x4261, 0x4261, 0x425f, 0x425f, 0x425f, 0x425f, 0x425f, 0x425e, 0x425e, 0x425c,
    0x425c, 0x425c, 0x425c, 0x425c, 0x425b, 0x425b, 0x425b, 0x4259, 0x4259, 0x4259, 0x4259, 0x4257,
    0x4257, 0x4257, 0x4256, 0x4256, 0x4256, 0x4256, 0x4256, 0x4253, 0x4253, 0x4253, 0x4253, 0x4253,
    0x4253, 0x4253, 0x4250, 0x4250, 0x4250, 0x4250, 0x4250, 0x424f, 0x424f, 0x424d, 0x424d, 0x424d,
    0x424d, 0x424d, 0x424b, 0x424b, 0x424b, 0x424b, 0x424b, 0x4249, 0x4249, 0x4249, 0x4248, 0x4248,
    0x4248, 0x4248, 0x4247, 0x4247, 0x4247, 0x4245, 0x4245, 0x4244, 0x4244, 0x4244, 0x4243, 0x4243,
    0x4241, 0x4241, 0x4241, 0x4240, 0x4240, 0x4240, 0x4240, 0x4240, 0x423e, 0x423e, 0x423e, 0x423e,
    0x423b, 0x423b, 0x423b, 0x423b, 0x423b, 0x423a, 0x423a, 0x423a, 0x4239, 0x4239, 0x4237, 0x4237,
    0x4237, 0x4236, 0x4236, 0x4236, 0x4236, 0x4236, 0x4235, 0x4235, 0x4234, 0x4234, 0x4232, 0x4232,
    0x4232, 0x4232, 0x4232, 0x4231, 0x4231, 0x4231, 0x422f, 0x422f, 0x422d, 0x422d, 0x422d, 0x422d,
    0x422d, 0x422c, 0x422c, 0x422c, 0x422a, 0x422a, 0x422a, 0x422a, 0x4228, 0x4228, 0x4228, 0x4228,
    0x4228, 0x4227, 0x4227, 0x4227, 0x4225, 0x4225, 0x4223, 0x4223, 0x4223, 0x4223, 0x4223, 0x4223,
    0x4223, 0x4221, 0x4220, 0x4220, 0x4220, 0x4220, 0x421f, 0x421f, 0x421f, 0x421d, 0x421d, 0x421d,
    0x421d, 0x421d, 0x421b, 0x421b, 0x421b, 0x421b, 0x421b, 0x4219, 0x4219, 0x4218, 0x4218, 0x4218,
    0x4218, 0x4218, 0x4215, 0x4215, 0x4215, 0x4215, 0x4215, 0x4215, 0x4215, 0x4213, 0x4213, 0x4213,
    0x4212, 0x4212, 0x4211, 0x4211, 0x4211, 0x420f, 0x420f, 0x420f, 0x420f, 0x420d, 0x420d, 0x420d,
    0x420c, 0x420c, 0x420c, 0x420c, 0x420c, 0x420a, 0x420a, 0x4209, 0x4209, 0x4209, 0x4209, 0x4209,
    0x4207, 0x4207, 0x4207, 0x4206, 0x4206, 0x4206, 0x4206, 0x4204, 0x4204, 0x4204, 0x4202, 0x4202,
    0x4202, 0x4202, 0x4202, 0x4201, 0x4201, 0x41fe, 0x41fe, 0x41fe, 0x41fe, 0x41fe, 0x41fb, 0x41fb,
    0x41fb, 0x41fb, 0x41f8, 0x41f8, 0x41f8, 0x41f8, 0x41f8, 0x41f4, 0x41f1, 0x41f1, 0x41f1, 0x41f1,
    0x41f1, 0x41f1, 0x41f1, 0x41ed, 0x41ed, 0x41ed, 0x41ed, 0x41ed, 0x41ea, 0x41ea, 0x41ea, 0x41e6,
    0x41e6, 0x41e6, 0x41e3, 0x41e3, 0x41e3, 0x41e3, 0x41e3, 0x41df, 0x41df, 0x41df, 0x41df, 0x41dc,
    0x41dc, 0x41dc, 0x41dc, 0x41dc, 0x41d8, 0x41d8, 0x41d8, 0x41d8, 0x41d5, 0x41d5, 0x41d5, 0x41d5,
    0x41d5, 0x41d1, 0x41d1, 0x41d1, 0x41cd, 0x41cd, 0x41cd, 0x41cd, 0x41cd, 0x41cd, 0x41cd, 0x41c9,
    0x41c9, 0x41c9, 0x41c6, 0x41c6, 0x41c6, 0x41c6, 0x41c6, 0x41c6, 0x41c6, 0x41c2, 0x41c2, 0x41be,
    0x41be, 0x41be, 0x41be, 0x41be, 0x41be, 0x41ba, 0x41ba, 0x41ba, 0x41ba, 0x41ba, 0x41b6, 0x41b6,
    0x41b6, 0x41b6, 0x41b6, 0x41b6, 0x41b6, 0x41b2, 0x41b2, 0x41ae, 0x41ae, 0x41ae, 0x41ae, 0x41ae,
    0x41ae, 0x41ae, 0x41ae, 0x41aa, 0x41aa, 0x41aa, 0x41aa, 0x41aa, 0x41a6, 0x41a6, 0x41a6, 0x41a6,
    0x41a6, 0x41a2, 0x41a2, 0x41a2, 0x41a2, 0x419e, 0x419e, 0x419e, 0x419e, 0x419e, 0x419e, 0x419a,
    0x419a, 0x419a, 0x419a, 0x419a, 0x4196, 0x4196, 0x4196, 0x4196, 0x4196, 0x4196, 0x4196, 0x4196,
    0x4196, 0x4192, 0x418e, 0x418e, 0x418e, 0x418e, 0x418e, 0x418e, 0x418e, 0x418e, 0x418e, 0x418a,
    0x418a, 0x418a, 0x418a, 0x418a, 0x4186, 0x4186, 0x4186, 0x4186, 0x4186, 0x4186, 0x4186, 0x4181,
    0x4181, 0x4181, 0x4181, 0x4181, 0x417a, 0x417a, 0x417a, 0x417a, 0x417a, 0x417a, 0x417a, 0x417a,
    0x4172, 0x4172, 0x4172, 0x4172, 0x4172, 0x4169, 0x4169, 0x4169, 0x4169, 0x4169, 0x4169, 0x4161,
    0x4161, 0x4161, 0x4161, 0x4161, 0x4161, 0x4158, 0x4158, 0x4158, 0x4158, 0x4158, 0x4158, 0x4158,
    0x4158, 0x4158, 0x414f, 0x414f, 0x414f, 0x414f, 0x414f, 0x4147, 0x4147, 0x4147, 0x4147, 0x4147,
    0x4147, 0x4147, 0x4147, 0x413e, 0x413e, 0x413e, 0x413e, 0x413e, 0x4135, 0x4135, 0x4135, 0x4135,
    0x4135, 0x4135, 0x4135, 0x4135, 0x4135, 0x412c, 0x412c, 0x412c, 0x412c, 0x412c, 0x412c, 0x4123,
    0x4123, 0x4123, 0x4123, 0x4123, 0x4123, 0x4123, 0x4123, 0x4123, 0x411a, 0x411a, 0x411a, 0x411a,
    0x411a, 0x411a, 0x4111, 0x4111, 0x4111, 0x4111, 0x4111, 0x4111, 0x4111, 0x4111, 0x4108, 0x4108,
    0x4108, 0x4108, 0x4108, 0x4108, 0x4108, 0x4108, 0x40ff, 0x40ff, 0x40ff, 0x40ff, 0x40ff, 0x40ff,
    0x40ff, 0x40ff, 0x40ed, 0x40ed, 0x40ed, 0x40ed, 0x40ed, 0x40ed, 0x40ed, 0x40ed, 0x40db, 0x40db,
    0x40db, 0x40db, 0x40db, 0x40db, 0x40db, 0x40db, 0x40c9, 0x40c9, 0x40c9, 0x40c9, 0x40c9, 0x40c9,
    0x40c9, 0x40c9, 0x40b7, 0x40b7, 0x40b7, 0x40b7, 0x40b7, 0x40b7, 0x40b7, 0x40b7, 0x40b7, 0x40b7,
    0x40a5, 0x40a5, 0x40a5, 0x40a5, 0x40a5, 0x40a5, 0x40a5, 0x4092, 0x4092, 0x4092, 0x4092, 0x4092,
    0x4092, 0x4092, 0x4092, 0x4092, 0x4080, 0x4080, 0x4080, 0x4080, 0x4080, 0x4080, 0x4080, 0x4080,
    0x4080, 0x405c, 0x405c, 0x405c, 0x405c, 0x405c, 0x405c, 0x405c, 0x405c, 0x405c, 0x4037, 0x4037,
    0x4037, 0x4037, 0x4037, 0x4037, 0x4037, 0x4037, 0x4037, 0x4013, 0x4013, 0x4013, 0x4013, 0x4013,
    0x4013, 0x4013, 0x4013, 0x4013, 0x4013, 0x3fdc, 0x3fdc, 0x3fdc, 0x3fdc, 0x3fdc, 0x3fdc, 0x3fdc,
    0x3fdc, 0x3fdc, 0x3fdc, 0x3f93, 0x3f93, 0x3f93, 0x3f93, 0x3f93, 0x3f93, 0x3f93, 0x3f93, 0x3f93,
    0x3f13, 0x3f13, 0x3f13, 0x3f13, 0x3f13, 0x3f13, 0x3f13, 0x3f13, 0x3f13, 0x3f13, 0x0,    0x0,
    0x0,    0x0,    0x0,    0x0,
};

// <! gen atan2 f(y, x) = 2 * atan(y / (pow(x*x+y*y, 0.5) + x))
static double _gen_atan2_degree(float y, float x) { return atan2(y, x) * 180 / M_PI; }

static void tl_lut_ref(uint16_t *ofmap, uint16_t *ifmap, uint16_t *ifmap2,
                       cvk_tl_shape_t ifmap_shape) {
  assert(ofmap);

  for (uint32_t i = 0; i < tl_shape_size(&ifmap_shape); i++) {
    float y = convert_bf16_fp32(ifmap2[i]);
    float x = convert_bf16_fp32(ifmap[i]);
    double v = _gen_atan2_degree(y, x);
    ofmap[i] = convert_fp32_bf16(v);

    if (mode == PRE_DATA_COMPARE_FIX) {
      ofmap[i] = golden_bf16[i];
    } else if (mode == DATA_COMPARE_U8) {
      ofmap[i] = (uint8_t)convert_bf16_s8(ofmap[i]);
    }
  }
}

static bool verify(uint16_t *ofmap_data, uint16_t *ref_data, uint16_t *ifmap, uint16_t *ifmap2,
                   uint64_t ifmap_size, float epsilon) {
  uint64_t size = ifmap_size;

  for (uint64_t i = 0; i < size; i++) {
    bool is_close;
    uint16_t ref = ref_data[i];
    uint16_t ofmap_data_bf16;
    float ref_f;
    float ofmap_data_f;

    ref_f = convert_bf16_fp32(ref);
    ofmap_data_f = convert_bf16_fp32(ofmap_data[i]);
    ofmap_data_bf16 = ofmap_data[i];

    if (mode == PRE_DATA_COMPARE_FIX) {
      is_close = ofmap_data[i] == ref;
    } else {
      is_close = fabs(ref_f - ofmap_data_f) < epsilon;
      if (abs(ofmap_data_f) * epsilon == 0) {
        // https://stackoverflow.com/questions/19837576/comparing-floating-point-number-to-zero
        is_close = abs(ref_f) < epsilon;
      } else {
        is_close = fabs(ref_f - ofmap_data_f) / fabs(std::max(ref_f, ofmap_data_f)) < epsilon;
      }
    }

    if (!is_close) {
      float y = convert_bf16_fp32(ifmap2[i]);
      float x = convert_bf16_fp32(ifmap[i]);
      fprintf(stderr,
              "comparing failed at ofmap_data[%lu]\n"
              "\tgot %x, exp %x, fp32: got %f exp %f, atan2(%f, %f) = %f"
              "\ty %f(0x%x), x %f(0x%x)\n",
              i, ofmap_data_bf16, ref, ofmap_data_f, ref_f, y, x, _gen_atan2_degree(y, x), y,
              ifmap2[i], x, ifmap[i]);
      exit(-1);
    }
  }

  return true;
}

static void _gen_input(uint16_t *input_data, uint64_t ifmap_size, int range_start, int range_end) {
  std::random_device rd;
  std::mt19937 e2(rd());
  std::uniform_real_distribution<> dist(range_start, range_end);

  float LO = pow(2, range_start);
  float HI = pow(2, range_end);
  for (uint64_t i = 0; i < ifmap_size; i++) {
    // input range is -8 ~ +8
    int table_hw = 256;
    float input =
        ((int)i % (range_end - 2)) * (((int)i % 2) ? 1 : -1) + 0.03 + (i % table_hw) * 0.002;
    input = ((int)i % (range_end - 2)) * (((int)i % 2) ? 1 : 1) + 0.03 + (i % table_hw) * 0.002;
    input_data[i] = convert_fp32_bf16(input);
    input = dist(e2);
    input = LO + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (HI - LO)));
  }
}

static void gen_input(uint16_t *x, uint16_t *y, uint64_t ifmap_size, TEST_MODE mode,
                      int range_start, int range_end) {
  if (mode == PRE_DATA_COMPARE_FIX) {
    memcpy(x, &test_pattern, sizeof(test_pattern));
  } else {
    range_start = abs(range_start);
    range_end = abs(range_end);
    _gen_input(x, ifmap_size, range_start, range_end);
  }

  // invert for test
  for (uint64_t i = 0; i < ifmap_size; i++) {
    y[i] = x[(ifmap_size - 1) - i];
  }

  if (mode == DATA_COMPARE_ACCURACY_X_GT_0) {
    // y = any
    uint32_t i = 0;
    for (; i < ifmap_size / 4; i++) {
      // y < 0
      y[i] = convert_fp32_bf16(-1 * convert_fp32_bf16(y[i]));
      y[i + ifmap_size / 4] = convert_fp32_bf16(0);
    }
  } else if (mode == DATA_COMPARE_ACCURACY_X_LT_0_Y_GE_0) {
    // x < 0 and y >= 0
    for (uint32_t i = 0; i < ifmap_size; i++) {
      x[i] = convert_fp32_bf16(-1 * convert_fp32_bf16(x[i]));
    }

    for (uint32_t i = 0; i < ifmap_size / 4; i++) {
      y[i + ifmap_size / 4] = convert_fp32_bf16(0);
    }
  } else if (mode == DATA_COMPARE_ACCURACY_X_LT_0_Y_LT_0) {
    // x < 0 and y < 0
    for (uint32_t i = 0; i < ifmap_size; i++) {
      x[i] = convert_fp32_bf16(-1 * convert_fp32_bf16(x[i]));
      y[i] = convert_fp32_bf16(-1 * convert_fp32_bf16(y[i]));
    }
  } else if (mode == DATA_COMPARE_ACCURACY_X_0_Y_GT_0) {
    // pi / 2, x = 0 and y > 0
    for (uint32_t i = 0; i < ifmap_size; i++) {
      x[i] = convert_fp32_bf16(0);
    }
  } else if (mode == DATA_COMPARE_ACCURACY_X_0_Y_LT_0) {
    // -pi / 2, x = 0 and y < 0
    for (uint32_t i = 0; i < ifmap_size; i++) {
      x[i] = convert_fp32_bf16(0);
      y[i] = convert_fp32_bf16(-1 * convert_fp32_bf16(y[i]));
    }
  }

  if (mode != PRE_DATA_COMPARE_FIX) {
    int i = 0;
    x[i] = convert_fp32_bf16(-10.0);
    y[i++] = convert_fp32_bf16(6.0);
    y[i] = convert_fp32_bf16(1.000000);
    x[i++] = convert_fp32_bf16(19.000000);
    y[i] = convert_fp32_bf16(5.000000);
    x[i++] = convert_fp32_bf16(-125.000000);
    y[i] = convert_fp32_bf16(1.070312);
    x[i++] = convert_fp32_bf16(0.498046);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-8.000000);
    x[i] = convert_fp32_bf16(424.000);
    y[i++] = convert_fp32_bf16(-1.00);
    x[i] = convert_fp32_bf16(2.484375);
    y[i++] = convert_fp32_bf16(-7.531250);
    x[i] = convert_fp32_bf16(-2.484375);
    y[i++] = convert_fp32_bf16(-7.531250);
    x[i] = convert_fp32_bf16(0);
    y[i++] = convert_fp32_bf16(7.531250);
    x[i] = convert_fp32_bf16(0);
    y[i++] = convert_fp32_bf16(-7.531250);
    x[i] = convert_fp32_bf16(0);
    y[i++] = convert_fp32_bf16(0);
    x[i] = convert_fp32_bf16(0);
    y[i++] = convert_fp32_bf16(0.394531);
    y[i] = convert_fp32_bf16(-4.000000);
    x[i++] = convert_fp32_bf16(-64.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-4.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-40.000000);
    y[i] = convert_fp32_bf16(1.000000);
    x[i++] = convert_fp32_bf16(-53.000000);
    y[i] = convert_fp32_bf16(-9.000000);
    x[i++] = convert_fp32_bf16(-91.000000);
    y[i] = convert_fp32_bf16(12.000000);
    x[i++] = convert_fp32_bf16(-164.000000);
    y[i] = convert_fp32_bf16(-20.000000);
    x[i++] = convert_fp32_bf16(-320.000000);
    y[i] = convert_fp32_bf16(1.000000);
    x[i++] = convert_fp32_bf16(-71.000000);
    y[i] = convert_fp32_bf16(1.000000);
    x[i++] = convert_fp32_bf16(-155.000000);
    y[i] = convert_fp32_bf16(1.000000);
    x[i++] = convert_fp32_bf16(-247.000000);
    y[i] = convert_fp32_bf16(-2.000000);
    x[i++] = convert_fp32_bf16(-118.000000);
    y[i] = convert_fp32_bf16(-2.000000);
    x[i++] = convert_fp32_bf16(-54.000000);
    y[i] = convert_fp32_bf16(-5.000000);
    x[i++] = convert_fp32_bf16(-392.000000);
    y[i] = convert_fp32_bf16(-37.000000);
    x[i++] = convert_fp32_bf16(-520.000000);
    y[i] = convert_fp32_bf16(-1.000000);
    x[i++] = convert_fp32_bf16(-19.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-10.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-8.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-2.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-14.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-2.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-6.000000);
    y[i] = convert_fp32_bf16(-1.000000);
    x[i++] = convert_fp32_bf16(-21.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-14.000000);
    y[i] = convert_fp32_bf16(-1.000000);
    x[i++] = convert_fp32_bf16(-17.000000);
    y[i] = convert_fp32_bf16(-1.000000);
    x[i++] = convert_fp32_bf16(-17.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-8.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-4.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-10.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-8.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-14.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-4.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-2.000000);
    y[i] = convert_fp32_bf16(1.000000);
    x[i++] = convert_fp32_bf16(-41.000000);
    y[i] = convert_fp32_bf16(1.000000);
    x[i++] = convert_fp32_bf16(-69.000000);
    y[i] = convert_fp32_bf16(4.000000);
    x[i++] = convert_fp32_bf16(-86.000000);
    y[i] = convert_fp32_bf16(1.000000);
    x[i++] = convert_fp32_bf16(-41.000000);
    y[i] = convert_fp32_bf16(-2.000000);
    x[i++] = convert_fp32_bf16(-34.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-6.000000);
    y[i] = convert_fp32_bf16(1.000000);
    x[i++] = convert_fp32_bf16(-41.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-136.000000);
    y[i] = convert_fp32_bf16(-3.000000);
    x[i++] = convert_fp32_bf16(-79.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-38.000000);
    y[i] = convert_fp32_bf16(5.000000);
    x[i++] = convert_fp32_bf16(-173.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-78.000000);
    y[i] = convert_fp32_bf16(-2.000000);
    x[i++] = convert_fp32_bf16(-60.000000);
    y[i] = convert_fp32_bf16(3.000000);
    x[i++] = convert_fp32_bf16(-123.000000);
    y[i] = convert_fp32_bf16(-9.000000);
    x[i++] = convert_fp32_bf16(-280.000000);
    y[i] = convert_fp32_bf16(3.000000);
    x[i++] = convert_fp32_bf16(-39.000000);
    y[i] = convert_fp32_bf16(2.000000);
    x[i++] = convert_fp32_bf16(-524.000000);
    y[i] = convert_fp32_bf16(11.000000);
    x[i++] = convert_fp32_bf16(-376.000000);
    y[i] = convert_fp32_bf16(5.000000);
    x[i++] = convert_fp32_bf16(-131.000000);
    y[i] = convert_fp32_bf16(11.000000);
    x[i++] = convert_fp32_bf16(-324.000000);
    y[i] = convert_fp32_bf16(9.000000);
    x[i++] = convert_fp32_bf16(-125.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-92.000000);
    y[i] = convert_fp32_bf16(-7.000000);
    x[i++] = convert_fp32_bf16(-233.000000);
    y[i] = convert_fp32_bf16(10.000000);
    x[i++] = convert_fp32_bf16(-170.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-4.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-4.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-10.000000);
    y[i] = convert_fp32_bf16(-1.000000);
    x[i++] = convert_fp32_bf16(-23.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-6.000000);
    y[i] = convert_fp32_bf16(0.000000);
    x[i++] = convert_fp32_bf16(-6.000000);
    y[i] = convert_fp32_bf16(-3.000000);
    x[i++] = convert_fp32_bf16(-37.000000);

    y[i] = convert_fp32_bf16(-9);
    x[i++] = convert_fp32_bf16(-1);

    y[i] = convert_fp32_bf16(7.0);
    x[i++] = convert_fp32_bf16(-1);

    y[i] = convert_fp32_bf16(0);
    x[i++] = convert_fp32_bf16(-1);
  }

#ifdef DBG
  for (uint64_t i = 0; i < ifmap_size; i++) {
    printf("source[%ld] y %f x %f\n", i, convert_bf16_fp32(y[i]), convert_bf16_fp32(x[i]));
  }
#endif /* ifdef DBG */
}

static void testbench(CVI_RT_HANDLE *ctx, cvk_context_t *bmk) {
  // TODO: check more shape / align
  cvk_chip_info_t chip_info = bmk->info;

  uint32_t input_n = 1;
  uint32_t input_c = chip_info.npu_num;
  uint32_t input_h = 16;
  uint32_t input_w = 16;
  float epsilon = 0.2;
  int range_start = -8;
  int range_end = 8;

  if (mode == PRE_DATA_COMPARE_FIX) {
    input_h = 4;
    input_w = 8;
  }

  cvk_fmt_t fmt = CVK_FMT_BF16;

  cvk_tl_shape_t ifmap_shape = {input_n, input_c, input_h, input_w};
  cvk_tl_shape_t ofmap_shape = ifmap_shape;

  // get lut table shape and size
  cvk_tl_shape_t table_shape;
  uint64_t table_bytesize = cvm_lut_tbl_bytesize(bmk, &table_shape, fmt);

  // get input / output size
  uint64_t ifmap_size = tl_shape_size(&ifmap_shape);
  uint64_t ofmap_size = tl_shape_size(&ofmap_shape);
  int data_type_size = bytesize_of_fmt(fmt);
  uint64_t ifmap_bytesize = ifmap_size * data_type_size;
  uint64_t ofmap_bytesize = ofmap_size * data_type_size;

  // atan2 was two inputs
  cvk_tl_t *tl_ifmap = test_alloc_tl(bmk, ifmap_shape, fmt, /*align*/ 1);
  cvk_tl_t *tl_ifmap2 = test_alloc_tl(bmk, ifmap_shape, fmt, /*align*/ 1);
  cvk_tl_t *tl_ofmap_bf16 = test_alloc_tl(bmk, ofmap_shape, fmt, /*align*/ 1);
  cvk_tl_t *out = tl_ofmap_bf16;

  // atan buf
  cvk_tl_t *tl_y0_buf = test_alloc_tl(bmk, table_shape, fmt, /*align*/ 1);
  cvk_tl_t *tl_invert_buf = test_alloc_tl(bmk, table_shape, fmt, /*align*/ 1);
  cvk_tl_t *tl_pos_neg_buf = test_alloc_tl(bmk, table_shape, fmt, /*align*/ 1);

  // reciprocal buf
  cvk_tl_t *tl_reciprocal_table_answer = test_alloc_tl(bmk, table_shape, fmt, /*align*/ 1);
  cvk_tl_t *tl_reciprocal_table_answer_mantissa = test_alloc_tl(bmk, table_shape, fmt, /*align*/ 1);

  // temp buf
  cvk_tl_t *tl_buf = test_alloc_tl(bmk, ofmap_shape, fmt, /*align*/ 1);
  cvk_tl_t *tl_buf2 = test_alloc_tl(bmk, tl_ofmap_bf16->shape, fmt, /*align*/ 1);
  cvk_tl_t *tl_buf3 = test_alloc_tl(bmk, tl_ofmap_bf16->shape, fmt, /*align*/ 1);

  uint16_t *input_data = (uint16_t *)xmalloc(ifmap_bytesize);
  uint16_t *input_data2 = (uint16_t *)xmalloc(ifmap_bytesize);
  uint16_t *ref_data = (uint16_t *)xmalloc(ofmap_bytesize);

  // for reciprocal
  uint16_t *table_reciprocal_data = (uint16_t *)xmalloc(table_bytesize);
  uint16_t *table_reciprocal_data_mantissa = (uint16_t *)xmalloc(table_bytesize);

  // for atan
  uint16_t *table_data_atan_y0 = (uint16_t *)xmalloc(table_bytesize);
  uint16_t *table_data_atan_invert = (uint16_t *)xmalloc(table_bytesize);
  uint16_t *table_data_atan_pos_neg = (uint16_t *)xmalloc(table_bytesize);

  // for search '0' index
  uint16_t *idx_0_table_data = (uint16_t *)xmalloc(table_bytesize);

  // init input / ref
  // input_data is x, input_data2 is y
  gen_input(input_data, input_data2, ifmap_size, mode, range_start, range_end);
  tl_lut_ref(ref_data, input_data, input_data2, ifmap_shape);

  // init lut table
  cvm_reciprocal_tbl(table_reciprocal_data, table_reciprocal_data_mantissa, &table_shape);
  cvm_atan_fast_degree_tbl(table_data_atan_y0, table_data_atan_invert, table_data_atan_pos_neg,
                           &table_shape);
  cvm_gen_0_tbl(idx_0_table_data, &table_shape);

  // sys->local
  test_put_tensor_g2l_comp(ctx, bmk, tl_ifmap, (uint8_t *)input_data);
  test_put_tensor_g2l_comp(ctx, bmk, tl_ifmap2, (uint8_t *)input_data2);
  test_put_tensor_g2l_comp(ctx, bmk, tl_reciprocal_table_answer, (uint8_t *)table_reciprocal_data);
  test_put_tensor_g2l_comp(ctx, bmk, tl_reciprocal_table_answer_mantissa,
                           (uint8_t *)table_reciprocal_data_mantissa);

  test_put_tensor_g2l_comp(ctx, bmk, tl_y0_buf, (uint8_t *)table_data_atan_y0);
  test_put_tensor_g2l_comp(ctx, bmk, tl_invert_buf, (uint8_t *)table_data_atan_invert);
  test_put_tensor_g2l_comp(ctx, bmk, tl_pos_neg_buf, (uint8_t *)table_data_atan_pos_neg);

  cvm_atan2_fast_degree_emit(bmk, tl_ifmap2, tl_ifmap, tl_buf, tl_buf2, tl_buf3, tl_y0_buf,
                             tl_invert_buf, tl_pos_neg_buf, tl_reciprocal_table_answer,
                             tl_reciprocal_table_answer_mantissa, OUT tl_ofmap_bf16, fmt);

  uint16_t *ofmap_data = (uint16_t *)test_get_tensor_l2g_comp(ctx, bmk, out);
  verify(ofmap_data, ref_data, input_data, input_data2, ifmap_size, epsilon);

  free_tl(bmk, tl_buf3);
  free_tl(bmk, tl_buf2);
  free_tl(bmk, tl_buf);
  free_tl(bmk, tl_reciprocal_table_answer_mantissa);
  free_tl(bmk, tl_reciprocal_table_answer);
  free_tl(bmk, tl_pos_neg_buf);
  free_tl(bmk, tl_invert_buf);
  free_tl(bmk, tl_y0_buf);
  free_tl(bmk, tl_ofmap_bf16);
  free_tl(bmk, tl_ifmap2);
  free_tl(bmk, tl_ifmap);

  free(table_data_atan_y0);
  free(idx_0_table_data);
  free(table_data_atan_invert);
  free(table_data_atan_pos_neg);
  free(table_reciprocal_data);
  free(table_reciprocal_data_mantissa);
  free(input_data);
  free(ref_data);
  free(ofmap_data);
  free(input_data2);
}

int main() {
  CVI_RT_HANDLE ctx;
  cvk_context_t *bmk;
  int round_mode;

  round_mode = set_store_feround();

  test_init(&ctx, &bmk);

  // for (int i = PRE_DATA_COMPARE_FIX; i < TEST_MODE_MAX; i++)
  // for (int i = PRE_DATA_COMPARE_FIX; i < DATA_COMPARE_ACCURACY; i++) {
  // for (int i = DATA_COMPARE_ACCURACY; i < DATA_COMPARE_U8; i++) {
  // for (int i = DATA_COMPARE_ACCURACY; i < DATA_COMPARE_ACCURACY_X_GT_0; i++)
  // {
  for (int i = PRE_DATA_COMPARE_FIX; i < DATA_COMPARE_U8; i++) {
    mode = static_cast<TEST_MODE>(i);
    printf("test mode %d...\n", mode);
    testbench(&ctx, bmk);
  }
  printf("pass\n");

  test_exit(&ctx, bmk);
  restore_feround(round_mode);
  return 0;
}
