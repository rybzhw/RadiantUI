// Copyright 2014 Joseph Riedel, All Rights Reserved.
// See LICENSE for licensing terms.

#include "RadiantUIPrivatePCH.h"
#include "RadiantCanvasRenderTarget.h"

URadiantCanvasRenderTarget* URadiantCanvasRenderTarget::CreateTransient(uint32 InSizeX, uint32 InSizeY, EPixelFormat InFormat, const FColor& InClearColor)
{
	URadiantCanvasRenderTarget* Canvas = nullptr;

	if ((InSizeX > 0) && (InSizeY > 0) &&
		((InSizeX % GPixelFormats[InFormat].BlockSizeX) == 0) &&
		((InSizeY % GPixelFormats[InFormat].BlockSizeY) == 0))
	{
		Canvas = NewObject<URadiantCanvasRenderTarget>(
			GetTransientPackage(),
			URadiantCanvasRenderTarget::StaticClass(),
			NAME_None,
			RF_Transient
			);

		Canvas->RenderTargetTexture = NewObject<UTextureRenderTarget2D>(
			GetTransientPackage(),
			UTextureRenderTarget2D::StaticClass(),
			NAME_None,
			RF_Transient
			);

		Canvas->RenderTargetTexture->ClearColor = InClearColor;
		Canvas->RenderTargetTexture->LODGroup = TEXTUREGROUP_UI;
		Canvas->RenderTargetTexture->CompressionSettings = TC_EditorIcon;
		Canvas->RenderTargetTexture->Filter = TF_Default;
		Canvas->RenderTargetTexture->InitCustomFormat(InSizeX, InSizeY, InFormat, true);
		//Canvas->RenderTargetTexture->UpdateResourceImmediate();
	}
	else
	{
		UE_LOG(RadiantUILog, Warning, TEXT("Invalid parameters specified for URadiantCanvasRenderTarget::Create()"));
	}

	return Canvas;
}

void URadiantCanvasRenderTarget::BeginPaint(float InRealTime, float InWorldTime, float InWorldDeltaTime, ERHIFeatureLevel::Type FeatureLevel)
{
	check(Canvas == nullptr);

	//ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER
	//(
	//	SetupRenderTargetViewport,
	//	FTextureRenderTarget2DResource*,
	//	RenderTarget,
	//	(FTextureRenderTarget2DResource*)RenderTargetTexture->GameThread_GetRenderTargetResource(),
	//	{
	//		SetRenderTarget(RHICmdList, RenderTarget->GetRenderTargetTexture(), FTexture2DRHIRef());
	//		RHICmdList.SetViewport(0, 0, 0.0f, RenderTarget->GetSizeXY().X, RenderTarget->GetSizeXY().Y, 1.0f);
	//	}
	//);

	// Setup the viewport
	ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER
	(
		SetupRenderTargetViewport,
		FTextureRenderTarget2DResource*,
		RenderTarget,
		(FTextureRenderTarget2DResource*)RenderTargetTexture->GameThread_GetRenderTargetResource(),
		{ 
			FRHIRenderPassInfo info(RenderTarget->GetRenderTargetTexture()->GetTexture2D(), ERenderTargetActions::DontLoad_Store);
			IRHICommandContext& c = RHICmdList.GetContext();
			c.RHIBeginRenderPass(info, TEXT("NameOfMyRenderpass"));
			RHICmdList.SetViewport(0, 0, 0.0f, RenderTarget->GetSizeXY().X, RenderTarget->GetSizeXY().Y, 1.0f);
			c.RHIEndRenderPass();
		}
	);


	Canvas = new (FCanvasBytes)FCanvas(RenderTargetTexture->GameThread_GetRenderTargetResource(), nullptr, InRealTime, InWorldTime, InWorldDeltaTime, FeatureLevel);
}

void URadiantCanvasRenderTarget::EndPaint()
{
	check(Canvas != nullptr);
	
	Canvas->Flush_GameThread();
	Canvas->~FCanvas();
	Canvas = nullptr;
	
	ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER
	(
		RenderTargetResolve,
		FTextureRenderTargetResource*,
		RenderTargetResource,
		RenderTargetTexture->GameThread_GetRenderTargetResource(),
		{
			RHICmdList.CopyToResolveTarget(RenderTargetResource->GetRenderTargetTexture(), RenderTargetResource->TextureRHI, FResolveParams());
		}
	);
}

void URadiantCanvasRenderTarget::Destroy()
{
	check(Canvas == nullptr);
	if (RenderTargetTexture)
	{
		RenderTargetTexture->MarkPendingKill();
	}

	MarkPendingKill();
}