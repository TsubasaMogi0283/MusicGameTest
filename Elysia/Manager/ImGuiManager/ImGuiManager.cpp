#include "ImGuiManager.h"

#include <imgui_impl_dx12.cpp>

#include "WindowsSetup.h"
#include "DirectXSetup.h"
#include "SrvManager.h"
#include "RtvManager.h"


Elysia::ImGuiManager* Elysia::ImGuiManager::GetInstance() {
	static ImGuiManager instance;
	return &instance;
}

void Elysia::ImGuiManager::Initialize() {

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();

	//フォントの設定
	std::string fontPath = "C:/Lesson/CG/CGGrade3/Ellysia_3.0/Resources/External/Font/GenShinGothic-P-Medium.ttf";
	ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	if (font == NULL) {
		assert(0);

	}

	ImGui_ImplWin32_Init(WindowsSetup::GetInstance()->GetHwnd());
	ImGui_ImplDX12_Init(
		DirectXSetup::GetInstance()->GetDevice().Get(),
		DirectXSetup::GetInstance()->GetSwapChain().swapChainDesc.BufferCount,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		SrvManager::GetInstance()->GetSrvDescriptorHeap().Get(),
		SrvManager::GetInstance()->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		SrvManager::GetInstance()->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

	// フォントテクスチャの作成
	ImGui_ImplDX12_CreateDeviceObjects();

}

void Elysia::ImGuiManager::BeginFrame() {
	//フレーム始まり
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

}




void Elysia::ImGuiManager::Draw() {
	//描画
	ImGui::Render();

	//描画用のDescriptorの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { SrvManager::GetInstance()->GetSrvDescriptorHeap().Get() };
	DirectXSetup::GetInstance()->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
}


void Elysia::ImGuiManager::EndDraw() {
	//コマンドを積む
	//実際のcommandListのImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DirectXSetup::GetInstance()->GetCommandList().Get());
}



void Elysia::ImGuiManager::Finalize() {
	//解放
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

