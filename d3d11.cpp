#include <windows.h>
#include <d3d11.h>
#include "MinHook.h"

typedef void(__stdcall *D3D11DRAWINDEXED) (ID3D11DeviceContext *pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
D3D11DRAWINDEXED orig_D3D11DrawIndexed = NULL;

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

//D3D11描画フック
void __stdcall hook_D3D11DrawIndexed(ID3D11DeviceContext *pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	ID3D11Buffer *veBuffer;
	UINT Stride;
	UINT veBufferOffset;
	D3D11_BUFFER_DESC veDesc;

	ID3D11Buffer *inBuffer;
	DXGI_FORMAT inFormat;
	UINT inOffset;
	D3D11_BUFFER_DESC inDesc;

	if (!flag) {
		return orig_D3D11DrawIndexed(pContext, IndexCount, StartIndexLocation, BaseVertexLocation);
	}

	pContext->IAGetVertexBuffers(0, 1, &veBuffer, &Stride, &veBufferOffset);
	if (veBuffer) {
		veBuffer->GetDesc(&veDesc);
	}
	if (NULL != veBuffer) {
		veBuffer->Release();
		veBuffer = NULL;
	}

	pContext->IAGetIndexBuffer(&inBuffer, &inFormat, &inOffset);
	if (inBuffer)
		inBuffer->GetDesc(&inDesc);
	if (NULL != inBuffer) {
		inBuffer->Release();
		inBuffer = NULL;
	}

	//テンノフードの裏はデフォルトの顔だからコメントアウト
	if (FALSE
		//////////////////////////////////////////////////////////////////////////////////////////
		|| (Stride == 32 && (FALSE
			//CYST
			|| (IndexCount == 96 && inDesc.ByteWidth == 6714 && veDesc.ByteWidth == 7680)
			//SARYN PRIME
			|| (IndexCount == 1692 && inDesc.ByteWidth == 43890 && veDesc.ByteWidth == 50752)
			|| (IndexCount == 40602 && inDesc.ByteWidth == 239520 && veDesc.ByteWidth == 272928)
			|| (IndexCount == 4080 && inDesc.ByteWidth == 24066 && veDesc.ByteWidth == 25216)
			|| (IndexCount == 1692 && inDesc.ByteWidth == 43890 && veDesc.ByteWidth == 50752)
			//SARYN ORPHID SKIN
			|| (IndexCount == 10716 && inDesc.ByteWidth == 63210 && veDesc.ByteWidth == 70240)
			//TRINITY SKIN
			|| (IndexCount == 2088 && inDesc.ByteWidth == 12312 && veDesc.ByteWidth == 13760)
			|| (IndexCount == 2772 && inDesc.ByteWidth == 16338 && veDesc.ByteWidth == 18688)
			|| (IndexCount == 1770 && inDesc.ByteWidth == 22740 && veDesc.ByteWidth == 25984)
			//TRINITY PRIME SKIN
			|| (IndexCount == 1710 && inDesc.ByteWidth == 20172 && veDesc.ByteWidth == 21632)
			|| (IndexCount == 7410 && inDesc.ByteWidth == 43710 && veDesc.ByteWidth == 53792)
			|| (IndexCount == 2088 && inDesc.ByteWidth == 12300 && veDesc.ByteWidth == 13504)
			//TRINITY STREGA SKIN
			|| (IndexCount == 4044 && inDesc.ByteWidth == 23826 && veDesc.ByteWidth == 26880)
			|| (IndexCount == 5256 && inDesc.ByteWidth == 30996 && veDesc.ByteWidth == 33984)
			|| (IndexCount == 13476 && inDesc.ByteWidth == 79482 && veDesc.ByteWidth == 83328)
			//BANSHEE PRIME SKIN
			|| (IndexCount == 666 && inDesc.ByteWidth == 4614 && veDesc.ByteWidth == 4608)
			|| (IndexCount == 834 && inDesc.ByteWidth == 5364 && veDesc.ByteWidth == 6560)
			|| (IndexCount == 1149 && inDesc.ByteWidth == 6942 && veDesc.ByteWidth == 10912)
			|| (IndexCount == 1464 && inDesc.ByteWidth == 8658 && veDesc.ByteWidth == 12224)
			//BANSHEE SOPRANA SKIN
			|| (IndexCount == 4332 && inDesc.ByteWidth == 25536 && veDesc.ByteWidth == 28544)
			|| (IndexCount == 6174 && inDesc.ByteWidth == 36402 && veDesc.ByteWidth == 40928)
			//TITANIA SKIN
			//まだ。ランタイムテッセレーションが悪さをする？
			//NEZHA SKIN
			|| (IndexCount == 2538 && inDesc.ByteWidth == 14958 && veDesc.ByteWidth == 15968)
			//CLOSED CENO HELMET
			//|| (IndexCount == 5844 && inDesc.ByteWidth == 53730 && veDesc.ByteWidth == 63136)
			//CLOSED KOPPRA HOOD
			//|| (IndexCount == 25362 && inDesc.ByteWidth == 149610 && veDesc.ByteWidth == 160256)
			//CLOSED MANDUKA HOOD
			//|| (IndexCount == 11310 && inDesc.ByteWidth == 66708 && veDesc.ByteWidth == 71744)
			//CLOSED SAITA PRIME HOOD
			//|| (IndexCount == 20106 && inDesc.ByteWidth == 118596 && veDesc.ByteWidth == 138304)
			//CLOSED VAHD MASK
			//|| (IndexCount == 8400 && inDesc.ByteWidth == 49554 && veDesc.ByteWidth == 54080)
			//CLOSED VARIDA HOOD
			//|| (IndexCount == 4356 && inDesc.ByteWidth == 165504 && veDesc.ByteWidth == 189792)
			//CLOSED ZARIMAN HOOD
			//|| (IndexCount == 23946 && inDesc.ByteWidth == 141264 && veDesc.ByteWidth == 153216)
			//CLOSED ZAUBA MASK
			//|| (IndexCount == 3792 && inDesc.ByteWidth == 22356 && veDesc.ByteWidth == 27104)
			))
		//////////////////////////////////////////////////////////////////////////////////////////
		|| (Stride == 24 && (FALSE
			//TRINITY MASK
			|| (IndexCount == 3582 && inDesc.ByteWidth == 21114 && veDesc.ByteWidth == 20592)
			//MAG MASK
			|| (IndexCount == 3348 && inDesc.ByteWidth == 19728 && veDesc.ByteWidth == 17496)
			))
		//////////////////////////////////////////////////////////////////////////////////////////
		) {
		return;
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

//初期化用スレッド
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	//よくわからんけど消すとCTD。
	//D3D11CreateDeviceの第一引数pAdapterをNULLにすると、まだ存在しないアダプタのリストを自動で読みに行って、アクセス違反で落ちる？
	do
	{
		Sleep(100);
	} while (!GetModuleHandle("dxgi.dll"));
	Sleep(1000);

	ID3D11Device *pDevice = NULL;
	ID3D11DeviceContext *pContext = NULL;

	/*
	D3D_DRIVER_TYPE_*
	REFERENCE, NULL: 実行時に要SDK。
	SOFTWARE: 引数SoftwareをNULLにできない。
	*/
	if (FAILED(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &pDevice, NULL, &pContext))) {
		return NULL;
	}

	vtable = *(void***)pContext;
	if (MH_OK != MH_Initialize()) {
		return 1;
	}
	if (MH_OK != MH_CreateHook(vtable[12], hook_D3D11DrawIndexed, (void**)&orig_D3D11DrawIndexed)) {
		return 1;
	}
	if (MH_OK != MH_EnableHook(vtable[12])) {
		return 1;
	}

	pDevice->Release();
	pContext->Release();

	return 0;
}

//エントリポイント
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	static HINSTANCE mHinstDLL;
	LPCSTR mImportNames[] = { "CreateDirect3D11DeviceFromDXGIDevice", "CreateDirect3D11SurfaceFromDXGISurface", "D3D11CoreCreateDevice", "D3D11CoreCreateLayeredDevice", "D3D11CoreGetLayeredDeviceSize", "D3D11CoreRegisterLayers", "D3D11CreateDevice", "D3D11CreateDeviceAndSwapChain", "D3D11CreateDeviceForD3D12", "D3D11On12CreateDevice", "D3DKMTCloseAdapter", "D3DKMTCreateAllocation", "D3DKMTCreateContext", "D3DKMTCreateDevice", "D3DKMTCreateSynchronizationObject", "D3DKMTDestroyAllocation", "D3DKMTDestroyContext", "D3DKMTDestroyDevice", "D3DKMTDestroySynchronizationObject", "D3DKMTEscape", "D3DKMTGetContextSchedulingPriority", "D3DKMTGetDeviceState", "D3DKMTGetDisplayModeList", "D3DKMTGetMultisampleMethodList", "D3DKMTGetRuntimeData", "D3DKMTGetSharedPrimaryHandle", "D3DKMTLock", "D3DKMTOpenAdapterFromHdc", "D3DKMTOpenResource", "D3DKMTPresent", "D3DKMTQueryAdapterInfo", "D3DKMTQueryAllocationResidency", "D3DKMTQueryResourceInfo", "D3DKMTRender", "D3DKMTSetAllocationPriority", "D3DKMTSetContextSchedulingPriority", "D3DKMTSetDisplayMode", "D3DKMTSetDisplayPrivateDriverFormat", "D3DKMTSetGammaRamp", "D3DKMTSetVidPnSourceOwner", "D3DKMTSignalSynchronizationObject", "D3DKMTUnlock", "D3DKMTWaitForSynchronizationObject", "D3DKMTWaitForVerticalBlankEvent", "D3DPerformance_BeginEvent", "D3DPerformance_EndEvent", "D3DPerformance_GetStatus", "D3DPerformance_SetMarker", "EnableFeatureLevelUpgrade", "OpenAdapter10", "OpenAdapter10_2" };
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstDLL);
		char sysdir[MAX_PATH];
		GetSystemDirectory(sysdir, sizeof(sysdir));
		mHinstDLL = LoadLibrary(lstrcat(sysdir, "\\d3d11.dll"));
		if (!mHinstDLL)
			return (FALSE);
		for (int i = 0; i < 51; i++) {
			mProcs[i] = GetProcAddress(mHinstDLL, mImportNames[i]);
		}
		//GetCurrentThreadIdがメインスレッドIDを取得できなくなるから、初期化用スレッドに移さないこと
		if (0 == hKeyHook) {
			hKeyHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, 0, GetCurrentThreadId());
		}
		CreateThread(0, 0, ThreadProc, 0, 0, 0);
		break;

	case DLL_PROCESS_DETACH:
		if (0 != hKeyHook) {
			UnhookWindowsHookEx(hKeyHook);
			hKeyHook = 0;
		}
		//初期化スレッドが成功する前にアンロードされたらCTDするからNULLチェック
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
