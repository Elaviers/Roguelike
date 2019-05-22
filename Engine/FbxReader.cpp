#include "IO.h"
#include "Engine.h"
//#include <fbxsdk.h>

/*

ModelData IO::ReadFBXFile(const char* filename)
{
	FbxImporter *importer = FbxImporter::Create(Engine::Instance().GetFBXManager(), "");

	if (importer->Initialize(filename, -1, Engine::Instance().GetFBXManager()->GetIOSettings()))
	{
		FbxScene* scene = FbxScene::Create(Engine::Instance().GetFBXManager(), "importScene");

		importer->Import(scene);

		importer->Destroy();

		FbxNode* root = scene->GetRootNode();
		if (root)
			for (int i = 0; i < root->GetChildCount(); ++i)
			{
				FbxNode *node = root->GetChild(i);

				

			}


		scene->Destroy();
	}

	return ModelData();
}
*/
