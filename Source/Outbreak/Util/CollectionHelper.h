#pragma once

class CollectionHelper
{
public:
	template<typename Type>
	static void ShuffleArray(FRandomStream& Stream, TArray<Type>& Array)
	{
		const int32 LastIndex = Array.Num() - 1;

		for (int32 i = 0; i <= LastIndex; i += 1)
		{
			const int32 Index = Stream.RandRange(i, LastIndex);
			if (i == Index)
			{
				continue;
			}

			Array.Swap(i, Index);
		}
	}

	template<typename Type>
	static Type GetRandomElementInArray(const TArray<Type>& Array)
	{
		check(Array.Num() > 0);
		int32 RandomIndex = FMath::RandRange(0, Array.Num() - 1);
		return Array[RandomIndex];
	}
};
