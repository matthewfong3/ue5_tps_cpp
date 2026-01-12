#pragma once

#define print(Text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT(Text), true);
#define printf_k(Key, Format, ...) if(GEngine) GEngine->AddOnScreenDebugMessage(Key, 5.0f, FColor::White, FString::Printf(TEXT(Format), ##__VA_ARGS__))