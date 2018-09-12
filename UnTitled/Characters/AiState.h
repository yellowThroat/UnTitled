#pragma once
#include "stdafx.h"

struct AiState
{
	wstring Name;
	float elapsedTime;
	bool bActive;

	function<void(void)> Starting;
	function<void(void)> Updating;
	function<void(void)> Finishing;

	bool IsActive() { return bActive; }

	void Reset()
	{
		bActive = false;
		elapsedTime = 0.0f;
	}

	void Update()
	{
		if (!bActive || Updating == NULL) return;
		
		elapsedTime += Time::Delta();

		Updating();
	}

	void Start()
	{
		if (Starting == NULL) return;

		Starting();

		bActive = true;
	}

	void Finish()
	{
		if (Finishing == NULL) return;

		Finishing();

		bActive = false;
	}
};