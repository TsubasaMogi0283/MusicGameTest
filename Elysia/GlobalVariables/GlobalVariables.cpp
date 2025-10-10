#include "GlobalVariables.h"

#include <imgui.h>

#include "ImGuiManager.h"
#include "WindowsSetup.h"


Elysia::GlobalVariables* Elysia::GlobalVariables::GetInstance(){
    static GlobalVariables instance;
    return &instance;
}

void Elysia::GlobalVariables::CreateGroup(const std::string& groupName){
    //指定名のオブジェクトが無ければ追加
    datas_[groupName];
}

void Elysia::GlobalVariables::SetValue(const std::string& groupName, const std::string& key,const int32_t& value){
    //グループの参照
    Group& group = datas_[groupName];
    
    //新しい項目のデータを設定
    Item newItem{};
    newItem.value = value;

    //設定した項目をstd::mapに追加
    group.items[key] = newItem;

}

void Elysia::GlobalVariables::SetValue(const std::string& groupName, const std::string& key,const float& value){
    //グループの参照
    Group& group = datas_[groupName];

    //新しい項目のデータを設定
    Item newItem{};
    newItem.value = value;

    //設定した項目をstd::mapに追加
    group.items[key] = newItem;

}

void Elysia::GlobalVariables::SetValue(const std::string& groupName, const std::string& key, const Vector2& value){
    //グループの参照
    Group& group = datas_[groupName];

    //新しい項目のデータを設定
    Item newItem{};
    newItem.value = value;

    //設定した項目をstd::mapに追加
    group.items[key] = newItem;
}

void Elysia::GlobalVariables::SetValue(const std::string& groupName, const std::string& key, const Vector3& value){
    //グループの参照
    Group& group = datas_[groupName];

    //新しい項目のデータを設定
    Item newItem{};
    newItem.value = value;

    //設定した項目をstd::mapに追加
    group.items[key] = newItem;

}

void Elysia::GlobalVariables::AddItem(const std::string& groupName, const std::string& key,const int32_t& value){
    

    //グループを検索
    std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);


    //無かったら止める
    assert(itGroup != datas_.end());

    
    //まず参照
    Group& group = itGroup->second;
    std::map<std::string, Item>::iterator itItem = group.items.find(key);
    //無かったら追加
    if (itItem == itGroup->second.items.end()) {
        SetValue(groupName, key, value);
    }
    
}

void Elysia::GlobalVariables::AddItem(const std::string& groupName, const std::string& key,const float& value){
    //グループを検索
    std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

    //無かったら止める
    assert(itGroup != datas_.end());


    //まず参照
    Group& group = itGroup->second;
    std::map<std::string, Item>::iterator itItem = group.items.find(key);
    //無かったら追加
    if (itItem == itGroup->second.items.end()) {
        SetValue(groupName, key, value);
    }
}

void Elysia::GlobalVariables::AddItem(const std::string& groupName, const std::string& key, const Vector2& value){
    //グループを検索
    std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

    //無かったら止める
    assert(itGroup != datas_.end());

    //まず参照
    Group& group = itGroup->second;
    std::map<std::string, Item>::iterator itItem = group.items.find(key);
    //無かったら追加
    if (itItem == itGroup->second.items.end()) {
        SetValue(groupName, key, value);
    }
}

void Elysia::GlobalVariables::AddItem(const std::string& groupName, const std::string& key, const Vector3& value){
    //グループを検索
    std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

    //無かったら止める
    assert(itGroup != datas_.end());

    //まず参照
    Group& group = itGroup->second;
    std::map<std::string, Item>::iterator itItem = group.items.find(key);
    //無かったら追加
    if (itItem == itGroup->second.items.end()) {
        SetValue(groupName, key, value);
    }
}

int32_t Elysia::GlobalVariables::GetIntValue(const std::string& groupName, const std::string& key) {
    //グループを検索
    std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

    //無かったら止める
    assert(itGroup != datas_.end());
    //参照
    Group& group = datas_.at(groupName);
    //キーを検索
    std::map<std::string, Item>::const_iterator itKey = group.items.find(key);
    //無かったら止める
    assert(itKey != group.items.end());
    //SaveFileより
    return std::get<int32_t>(itKey->second.value);



}

float Elysia::GlobalVariables::GetFloatValue(const std::string& groupName, const std::string& key) {
    //グループを検索
    std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

    //無かったら止める
    assert(itGroup != datas_.end());
    //参照
    Group& group = datas_.at(groupName);
    //キーを検索
    std::map<std::string, Item>::const_iterator itKey = group.items.find(key);
    //無かったら止める
    assert(itKey != group.items.end());
    //SaveFileより
    return std::get<float>(itKey->second.value);

}

Vector2 Elysia::GlobalVariables::GetVector2Value(const std::string& groupName, const std::string& key){

    //グループを検索
    std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

    //無かったら止める
    assert(itGroup != datas_.end());
    //参照
    Group& group = datas_.at(groupName);
    //キーを検索
    std::map<std::string, Item>::const_iterator itKey = group.items.find(key);
    //無かったら止める
    assert(itKey != group.items.end());
    //SaveFileより
    return std::get<Vector2>(itKey->second.value);


}

Vector3 Elysia::GlobalVariables::GetVector3Value(const std::string& groupName, const std::string& key) {
    //グループを検索
    std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);
    
    //無かったら止める
    assert(itGroup != datas_.end());
    //参照
    Group& group = datas_.at(groupName);
    //キーを検索
    std::map<std::string, Item>::const_iterator itKey = group.items.find(key);
    //無かったら止める
    assert(itKey != group.items.end());
    //SaveFileより
    return std::get<Vector3>(itKey->second.value);


}

void Elysia::GlobalVariables::SaveFile(const std::string& groupName){
    //グループを検索
    std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

    //無かったら止める
    assert(itGroup!=datas_.end());

    nlohmann::json root;
    //json::objectはstd::mapみたいなもの
    root = nlohmann::json::object();

    //jsonオブジェクト登録
    root[groupName] = nlohmann::json::object();

    //各項目について
    for (std::map<std::string, Item>::iterator itItem = itGroup->second.items.begin();
        itItem != itGroup->second.items.end(); ++itItem) {
        //項目名を取得
        const std::string& itemName = itItem->first;
        //項目の参照を取得
        Item& item = itItem->second;


        //int32_tの場合
        if (std::holds_alternative<int32_t>(item.value) == true) {
            //int32_t型の値を登録
            root[groupName][itemName] = std::get<int32_t>(item.value);
        }
        //floatの場合
        else if (std::holds_alternative<float>(item.value) == true) {
            //float型の値を登録
            root[groupName][itemName] = std::get<float>(item.value);
        }
        //Vector2の場合
        else if (std::holds_alternative<Vector2>(item.value) == true) {
            //float型のjson配列登録
            Vector2 value = std::get<Vector2>(item.value);
            root[groupName][itemName] = nlohmann::json::array({ value.x, value.y});
        }
        //Vector3の場合
        else if (std::holds_alternative<Vector3>(item.value) == true) {
            //float型のjson配列登録
            Vector3 value = std::get<Vector3>(item.value);
            root[groupName][itemName] = nlohmann::json::array({value.x, value.y, value.z});
        }

    }


    //ディレクトリが無ければ作成する
    std::filesystem::path dir(DIRECTORY_PATH_);
    if (!std::filesystem::exists(DIRECTORY_PATH_)) {
        std::filesystem::create_directories(DIRECTORY_PATH_);
    }

    //書き込むJSONファイルのフルパスを合成する
    std::string filePath = DIRECTORY_PATH_ + groupName + ".json";
    //書き込む用ファイルストリーム
    std::ofstream ofs;
    //ファイルを書き込み用に開く
    ofs.open(filePath);

    //ファイルを開くのに失敗した場合
    if (ofs.fail()) {
        //書き込み用のファイルを開けるのに失敗したよ
        std::string message = "Failed open data file for write.";
        //WindowsAPIのメッセージボックスをここで使う
        MessageBoxA(nullptr, message.c_str(), "AdjustmentItems", 0u);
        assert(0);
        return;
    }

    //ファイルにjson文字列を書き込む(インデント幅4)
    //rootオブジェクトに格納されている全データをJSON文字列に変換して、
    //書き込み用のファイルに流し込む。
    ofs << std::setw(4) << root << std::endl;


    //書き込み用に開いたファイルを閉じる。
    ofs.close();


}

void Elysia::GlobalVariables::LoadAllFile(){
    //保存先ディレクトリのパスをローカル変数で宣言する
    std::filesystem::path directory(DIRECTORY_PATH_);

    //無かったらreturn
    if (!std::filesystem::exists(DIRECTORY_PATH_)) {
        return;
    }

    //各ファイルの処理
    std::filesystem::directory_iterator dirIt(DIRECTORY_PATH_);
    //directory_iterator...ディレクトリないのファイルを1つずつ指すためのポインタのようなもの
    for (const std::filesystem::directory_entry& entry : dirIt) {

        //パスによる判別
        //ファイルパスを取得
        const std::filesystem::path& filePath = entry.path();

        //ファイル拡張子を取得
        std::string extension = filePath.extension().string();
        //.jsonファイル以外はスキップ
        if (extension.compare(".json") != 0) {
            continue;
        }

        //extension...拡張子のみ抽出
        //stem...拡張子を除くファイル名を抽出
        //ファイル読み込み
        LoadFile(filePath.stem().string());

    }



}

void Elysia::GlobalVariables::LoadFile(const std::string& groupName){
    //読み込むJSONファイルのフルパスを合成する
    std::string filePath = DIRECTORY_PATH_ + groupName + ".json";
    //読み込み用のファイルストリーム
    std::ifstream ifs;
    ifs.open(filePath);

    //書き込みはofstream
    //読み込みはifstream

    //読み込みが失敗した場合
    if (ifs.fail()) {
        //読み込み用のファイルを開けるのに失敗したよ
        std::string message = "Failed open data file for read.";
        //WindowsAPIのメッセージボックスをここで使う
        MessageBoxA(nullptr, message.c_str(), "AdjustmentItems", 0u);
        assert(0);
        return;
    }

    nlohmann::json root = {};

    //json文字列からjsonのデータ構造に展開
    ifs >> root;
    //ファイルを閉じる
    ifs.close();

    //グループ登録確認
    //グループを検索
    nlohmann::json::iterator itGroup = root.find(groupName);

    //未登録チェック
    assert(itGroup != root.end());

    //確認アイテムについて
    for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
        //アイテム名(キー)を取得
        const std::string& itemName = itItem.key();
    
        if(itItem->is_number_integer()){
            //int型の値を登録
            int32_t value = itItem->get<int32_t>();
            SetValue(groupName, itemName, value);
        }
        else if (itItem->is_number_float()) {
            //float型の値を登録
            //floatは無いのでその代わりdoubleで
            double value = itItem->get<double>();
            SetValue(groupName, itemName, static_cast<float>(value));
        }
        //要素数2の配列であれば
        else if (itItem->is_array() && itItem->size() == 2) {
            //float型のison配列登録
            Vector2 value = {.x = itItem->at(0).get<float>(),.y = itItem->at(1).get<float>()};
            SetValue(groupName, itemName, value);
        }
        //要素数3の配列であれば
        else if (itItem->is_array() && itItem->size() == 3) {
            //float型のison配列登録
            Vector3 value = {.x = itItem->at(0).get<float>(),.y = itItem->at(1).get<float>(),.z = itItem->at(2).get<float>() };
            SetValue(groupName, itemName, value);
        }
    }
}

void Elysia::GlobalVariables::Update(){
#ifdef _DEBUG

    //メニューバーを使用可能なフラグを付けてウィンドウを開く
    if (!ImGui::Begin("調整項目")) {
        ImGui::End();
        return;
    }

    for (std::map<std::string, Group>::iterator itGroup = datas_.begin(); itGroup != datas_.end(); ++itGroup) {

        //イテレータはペア1組分を指すポインタのようなもの
        //first...キー
        //second...値

        //グループ名を取得
        const std::string& groupName = itGroup->first;
        //グループの値を参照
        Group& group = itGroup->second;

        //グループメニューを追加する処理
        if (!ImGui::TreeNode(groupName.c_str())) {
            continue;
        }

        //各項目について
        for (std::map<std::string, Item>::iterator itItem = group.items.begin(); 
            itItem != group.items.end(); ++itItem) {
            //項目名を取得
            const std::string& itItemName = itItem->first;
            //項目の参照を取得
            Item& item = itItem->second;

            //alternative...代替
            //「std::holds_alternative」で型の判別が出来るよ

            //int32_t型を持っている場合
            if (std::holds_alternative<int32_t>(item.value) == true) {
                //違いに注意！
                //「get」で値を取得
                //「get_if」でポインタを取得
                int32_t* ptr = std::get_if<int32_t>(&item.value);
                ImGui::InputInt(itItemName.c_str(), ptr, 0, 100);
            }
            //float型を持っている場合
            else if (std::holds_alternative<float>(item.value) == true) {
                //ポインタの取得
                float* ptr = std::get_if<float>(&item.value);
                ImGui::InputFloat(itItemName.c_str(), ptr);

            }
            //Vector2型を持っている場合
            else if (std::holds_alternative<Vector2>(item.value)==true) {
                //ポインタの取得
                Vector2* ptr = std::get_if<Vector2>(&item.value);
                //ここではVector3をfloatの配列ということにする
                ImGui::InputFloat2(itItemName.c_str(), reinterpret_cast<float*>(ptr));
            }
            //Vector3型を持っている場合
            else if (std::holds_alternative<Vector3>(item.value)==true) {
                //ポインタの取得
                Vector3* ptr = std::get_if<Vector3>(&item.value);
                //ここではVector3をfloatの配列ということにする
                ImGui::InputFloat3(itItemName.c_str(), reinterpret_cast<float*>(ptr));
            }
        }

        //セーブボタン
        ImGui::Text("\n");
        if (ImGui::Button("Save")==true) {
            SaveFile(groupName);
            std::string message = std::format("{}, json saved.", groupName);
            MessageBoxA(nullptr, message.c_str(), "AdjustmentItems", 0u);
        }

        ImGui::TreePop();

    }

    ImGui::End();

#endif 
}
