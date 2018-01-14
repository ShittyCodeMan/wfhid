#include <windows.h>
#include <imagehlp.h>
#include <d3d11.h>
#include "MinHook.h"

typedef void(__stdcall *D3D11DRAWINDEXED) (ID3D11DeviceContext *pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
D3D11DRAWINDEXED orig_D3D11DrawIndexed = NULL;
typedef struct FILTER_ENTRY{
	UINT IndexCount, inDesc, veDesc;
} _FILTER_ENTRY;
typedef struct FILTER_HASH {
	FILTER_ENTRY entry[2]; //Vector採用は保留
	int length;
} _FILTER_HASH;
typedef struct FILTER_STRIDE {
	FILTER_HASH hash[256];
} _FILTER_STRIDE;
typedef struct FILTER_TABLE {
	FILTER_STRIDE Stride24;
	FILTER_STRIDE Stride32;
} _FILTER_TABLE;


extern "C" {
	void *mProcs[51] = { 0 };

	void CreateDirect3D11DeviceFromDXGIDevice_wrapper();
	void CreateDirect3D11SurfaceFromDXGISurface_wrapper();
	void D3D11CoreCreateDevice_wrapper();
	void D3D11CoreCreateLayeredDevice_wrapper();
	void D3D11CoreGetLayeredDeviceSize_wrapper();
	void D3D11CoreRegisterLayers_wrapper();
	void D3D11CreateDevice_wrapper();
	void D3D11CreateDeviceAndSwapChain_wrapper();
	void D3D11CreateDeviceForD3D12_wrapper();
	void D3D11On12CreateDevice_wrapper();
	void D3DKMTCloseAdapter_wrapper();
	void D3DKMTCreateAllocation_wrapper();
	void D3DKMTCreateContext_wrapper();
	void D3DKMTCreateDevice_wrapper();
	void D3DKMTCreateSynchronizationObject_wrapper();
	void D3DKMTDestroyAllocation_wrapper();
	void D3DKMTDestroyContext_wrapper();
	void D3DKMTDestroyDevice_wrapper();
	void D3DKMTDestroySynchronizationObject_wrapper();
	void D3DKMTEscape_wrapper();
	void D3DKMTGetContextSchedulingPriority_wrapper();
	void D3DKMTGetDeviceState_wrapper();
	void D3DKMTGetDisplayModeList_wrapper();
	void D3DKMTGetMultisampleMethodList_wrapper();
	void D3DKMTGetRuntimeData_wrapper();
	void D3DKMTGetSharedPrimaryHandle_wrapper();
	void D3DKMTLock_wrapper();
	void D3DKMTOpenAdapterFromHdc_wrapper();
	void D3DKMTOpenResource_wrapper();
	void D3DKMTPresent_wrapper();
	void D3DKMTQueryAdapterInfo_wrapper();
	void D3DKMTQueryAllocationResidency_wrapper();
	void D3DKMTQueryResourceInfo_wrapper();
	void D3DKMTRender_wrapper();
	void D3DKMTSetAllocationPriority_wrapper();
	void D3DKMTSetContextSchedulingPriority_wrapper();
	void D3DKMTSetDisplayMode_wrapper();
	void D3DKMTSetDisplayPrivateDriverFormat_wrapper();
	void D3DKMTSetGammaRamp_wrapper();
	void D3DKMTSetVidPnSourceOwner_wrapper();
	void D3DKMTSignalSynchronizationObject_wrapper();
	void D3DKMTUnlock_wrapper();
	void D3DKMTWaitForSynchronizationObject_wrapper();
	void D3DKMTWaitForVerticalBlankEvent_wrapper();
	void D3DPerformance_BeginEvent_wrapper();
	void D3DPerformance_EndEvent_wrapper();
	void D3DPerformance_GetStatus_wrapper();
	void D3DPerformance_SetMarker_wrapper();
	void EnableFeatureLevelUpgrade_wrapper();
	void OpenAdapter10_wrapper();
	void OpenAdapter10_2_wrapper();
}

HHOOK hKeyHook = 0;
void **vtable = NULL;
BOOL flag = FALSE;
FILTER_TABLE filter;


//D3D11描画フック
void __stdcall hook_D3D11DrawIndexed(ID3D11DeviceContext *pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	ID3D11Buffer *veBuffer;
	UINT veWidth;
	UINT Stride;
	UINT veBufferOffset;
	D3D11_BUFFER_DESC veDesc;

	ID3D11Buffer *inBuffer;
	UINT inWidth;
	DXGI_FORMAT inFormat;
	UINT inOffset;
	D3D11_BUFFER_DESC inDesc;

	if (!flag) {
		return orig_D3D11DrawIndexed(pContext, IndexCount, StartIndexLocation, BaseVertexLocation);
	}

	pContext->IAGetVertexBuffers(0, 1, &veBuffer, &Stride, &veBufferOffset);
	if (veBuffer) {
		veBuffer->GetDesc(&veDesc);
		veWidth = veDesc.ByteWidth;
	}
	if (NULL != veBuffer) {
		veBuffer->Release();
		veBuffer = NULL;
	}

	pContext->IAGetIndexBuffer(&inBuffer, &inFormat, &inOffset);
	if (inBuffer) {
		inBuffer->GetDesc(&inDesc);
		inWidth = inDesc.ByteWidth;
	}
	if (NULL != inBuffer) {
		inBuffer->Release();
		inBuffer = NULL;
	}

	FILTER_HASH *phash = &(Stride == 32 ? filter.Stride32 : filter.Stride24).hash[inWidth & 0xFF];
	FILTER_ENTRY *pentry;
	for (int i = phash->length - 1; i >= 0; i--) {
		pentry = &phash->entry[i];
		if (pentry->IndexCount == IndexCount && pentry->inDesc == inWidth && pentry->veDesc == veWidth) {
			return;
		}
	}

	return orig_D3D11DrawIndexed(pContext, IndexCount, StartIndexLocation, BaseVertexLocation);
}

//キーボードフック
LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	if (0 < code) {
		return CallNextHookEx(hKeyHook, code, wParam, lParam);
	}
	if (HC_ACTION == code) {
		if (VK_PAUSE == wParam && lParam >> 31) {
			flag = !flag;
			MessageBeep(flag ? MB_ICONEXCLAMATION : MB_ICONQUESTION);
		}
	}
	return CallNextHookEx(hKeyHook, code, wParam, lParam);
}

void InsertFilter(FILTER_STRIDE *fstride, UINT IndexCount, UINT inWidth, UINT veWidth) {
	FILTER_HASH *phash;
	FILTER_ENTRY *pentry;
	phash = &fstride->hash[inWidth & 0xFF];
	pentry = &phash->entry[phash->length++];
	pentry->IndexCount = IndexCount;
	pentry->inDesc = inWidth;
	pentry->veDesc = veWidth;
}

FILTER_TABLE GenerateFilterTable() {
	FILTER_TABLE r;
	SecureZeroMemory(&r, sizeof(r));

#define INSERT_FILTER(IndexCount, inWidth, veWidth) InsertFilter(&r.Stride32, IndexCount, inWidth, veWidth)
	//CYST
	INSERT_FILTER(96, 6714, 7680);
	INSERT_FILTER(1008, 6714, 7680);
	//SARYN PRIME
	INSERT_FILTER(40602, 239520, 272928);
	INSERT_FILTER(4080, 24066, 25216);
	INSERT_FILTER(4080, 24054, 25216);
	INSERT_FILTER(1692, 43890, 50752);
	INSERT_FILTER(5748, 43890, 50752);
	//SARYN ORPHID SKIN
	INSERT_FILTER(10716, 63210, 70240);
	//TRINITY SKIN
	INSERT_FILTER(2088, 12312, 13760);
	INSERT_FILTER(2772, 16338, 18688);
	INSERT_FILTER(1770, 22740, 25984);
	INSERT_FILTER(1044, 22740, 25984);
	//TRINITY PRIME SKIN
	INSERT_FILTER(1710, 20172, 21632);
	INSERT_FILTER(7410, 43710, 53792);
	INSERT_FILTER(2088, 12300, 13504);
	INSERT_FILTER(20460, 197052, 233728);
	//TRINITY STREGA SKIN
	INSERT_FILTER(4044, 23826, 26880);
	INSERT_FILTER(5256, 30996, 33984);
	INSERT_FILTER(13476, 79482, 83328);
	//BANSHEE PRIME SKIN
	INSERT_FILTER(666, 4614, 4608);
	INSERT_FILTER(576, 4320, 4352);
	INSERT_FILTER(834, 5364, 6560);
	INSERT_FILTER(1149, 6942, 10912);
	INSERT_FILTER(1464, 8658, 12224);
	//BANSHEE SOPRANA SKIN
	INSERT_FILTER(4332, 25536, 28544);
	INSERT_FILTER(6174, 36402, 40928);
	//TITANIA SKIN
	INSERT_FILTER(2586, 15234, 17376);
	INSERT_FILTER(2586, 15222, 17376);
	INSERT_FILTER(6228, 36720, 41472);
	INSERT_FILTER(12084, 71292, 77760);
	INSERT_FILTER(12084, 71280, 77760);
	INSERT_FILTER(2958, 349944, 393184);
	//NEZHA SKIN
	INSERT_FILTER(2538, 14958, 15968);
	//MIRAGE PRIME
	INSERT_FILTER(4920, 29016, 33408);
	INSERT_FILTER(8184, 48264, 53888);
	INSERT_FILTER(1716, 245262, 262496);
	//EMBER VERMILLION SKIN
	INSERT_FILTER(1656, 9750, 11968);
	INSERT_FILTER(2670, 15720, 17120);
	INSERT_FILTER(2670, 15738, 17120);
	INSERT_FILTER(636, 4512, 4992);
	INSERT_FILTER(636, 4524, 4992);
	INSERT_FILTER(2544, 14994, 19968);
	INSERT_FILTER(2544, 14988, 19968);
	//CLOSED CENO HELMET
	INSERT_FILTER(5844, 53730, 63136);
	//CLOSED KOPPRA HOOD
	INSERT_FILTER(25362, 149610, 160256);
	//CLOSED MANDUKA HOOD
	INSERT_FILTER(11310, 66708, 71744);
	//CLOSED SAITA PRIME HOOD
	INSERT_FILTER(20106, 118596, 138304);
	//CLOSED VAHD MASK
	INSERT_FILTER(8400, 49554, 54080);
	//CLOSED VARIDA HOOD
	INSERT_FILTER(4356, 165504, 189792);
	//CLOSED ZARIMAN HOOD
	INSERT_FILTER(23946, 141264, 153216);
	//CLOSED ZAUBA MASK
	INSERT_FILTER(3792, 22356, 27104);
#undef INSERT_FILTER

#define INSERT_FILTER(IndexCount, inWidth, veWidth) InsertFilter(&r.Stride24, IndexCount, inWidth, veWidth)
	//TRINITY MASK
	INSERT_FILTER(3582, 21114, 20592);
	//MAG MASK
	INSERT_FILTER(3348, 19728, 17496);
	//EXCALIBUR MASK
	INSERT_FILTER(3588, 21144, 18720);
	//VOLT MASK
	INSERT_FILTER(2430, 14316, 13224);
#undef INSERT_FILTER

	return r;
}

//初期化用スレッド
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	filter = GenerateFilterTable();

	//D3D11フック開始

	//よくわからんけど消すとCTD。
	//D3D11CreateDeviceの第一引数pAdapterをNULLにすると、まだ存在しないアダプタのリストを自動で読みに行って、アクセス違反で落ちる？
	//	pAdapterを指定してD3D_DRIVER_TYPE_UNKNOWNしても落ちた。
	do
	{
		Sleep(100);
	} while (!GetModuleHandle("dxgi.dll"));
	Sleep(7000);

	/*
	IDXGIFactory1* pFactory;
	IDXGIAdapter1* pAdapter;
	if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&pFactory)))) {
		return 1;
	}

	if (FAILED(pFactory->EnumAdapters1(0, &pAdapter))) {
		pFactory->Release();
		return 1;
	}
	*/

	ID3D11Device *pDevice = NULL;
	ID3D11DeviceContext *pContext = NULL;

	/*
	D3D_DRIVER_TYPE_*
	REFERENCE, NULL: 実行時に要SDK。
	SOFTWARE: 引数SoftwareをNULLにできない。
	*/
	if (FAILED(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &pDevice, NULL, &pContext))) {
		return 1;
	}

	vtable = *(void***)pContext;
	MH_STATUS st;
	if (MH_OK != (st = MH_Initialize()) && MH_ERROR_ALREADY_INITIALIZED != st) {
		MessageBox(0, "MH_Initialize", NULL, MB_OK);
		return 1;
	}
	if (MH_OK != (st = MH_CreateHook(vtable[12], hook_D3D11DrawIndexed, (void**)&orig_D3D11DrawIndexed)) && MH_ERROR_ALREADY_CREATED != st) {
		MessageBox(0, "MH_CreateHook", NULL, MB_OK);
		return 1;
	}
	if (MH_OK != (st = MH_EnableHook(vtable[12])) && MH_ERROR_ENABLED != st) {
		MessageBox(0, "MH_EnableHook", NULL, MB_OK);
		return 1;
	}

	pDevice->Release();
	pContext->Release();

	return 0;
}

//エントリポイント
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	static HINSTANCE mHinstDLL;
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstDLL);
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
		char sysdir[MAX_PATH];
		GetSystemDirectory(sysdir, sizeof(sysdir));
		mHinstDLL = LoadLibrary(lstrcat(sysdir, "\\d3d11.dll"));
		if (!mHinstDLL)
			return (FALSE);

		//全てのエクスポート関数の序数が連番である場合のみ動作する。
		//エクスポート関数の総数が変わる場合は格納先の配列(mProcs)の長さも変更すること。
		char *BaseAddr;
		ULONG Size;
		PIMAGE_EXPORT_DIRECTORY pExport;
		DWORD *pNames;
		WORD *pOrdis;
		BaseAddr = (char*)hinstDLL;
		pExport = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryEntryToData(BaseAddr, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &Size);
		pNames = (DWORD*)(BaseAddr + pExport->AddressOfNames);
		pOrdis = (WORD*)(BaseAddr + pExport->AddressOfNameOrdinals);
		for (DWORD i = 0, l = pExport->NumberOfFunctions; i < l; i++) {
			mProcs[pOrdis[i]] = GetProcAddress(mHinstDLL, BaseAddr + pNames[i]);
		}

		//lpParameterに(void*)GetCurrentThreadId()してみたけど、うまくいかなかった。
		//Toolhelpで最初に列挙されるスレッドエントリはメインスレッドだと、仕様として定義されてるとかされてないとかどっかで読んだ覚えがある。
		//でもそもそもWindowsにはメインスレッドという概念すらなかったような・・・。
		//スレッド開始時間でメインスレッドを特定できる。
		//単純にグローバル変数にGetCurrentThreadId()を代入しておけばいい気がする。
		//別にそこまでDLLローダの実行時間を気にしなくていい気がする。動くんだからこれでいい。
		if (0 == hKeyHook) {
			hKeyHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, 0, GetCurrentThreadId());
		}

		CreateThread(0, 0, ThreadProc, NULL, 0, 0);
		break;

	case DLL_PROCESS_DETACH:
		if (0 != hKeyHook) {
			UnhookWindowsHookEx(hKeyHook);
			hKeyHook = 0;
		}
		//初期化スレッドが成功する前にアンロードされたらCTDするからNULLチェック。
		if (vtable && MH_OK != MH_DisableHook(vtable[12])) {
			return FALSE;
		}
		FreeLibrary(mHinstDLL);
		break;

	default:
		break;
	}
	return (TRUE);
}
