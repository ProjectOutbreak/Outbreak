#pragma once

class AsynchronousLoadingHelper
{
public:
	template<typename T>
	static void AppendPaths(const TArray<TSoftObjectPtr<T>>& In, TArray<FSoftObjectPath>& Out)
	{
		for (const TSoftObjectPtr<T>& Ptr : In)
		{
			const FSoftObjectPath& Path = Ptr.ToSoftObjectPath();
			if (Path.IsValid())
				Out.AddUnique(Path);
		}
	}
};
