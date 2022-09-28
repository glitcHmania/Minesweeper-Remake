#include "Minefield.h"
#include <assert.h>
#include <algorithm>

Minefield::Minefield(int in_nMines)
{
	nMines = in_nMines;
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> xDist(0, width - 1);
	std::uniform_int_distribution<int> yDist(0, height - 1);

	for (int minesSpawned = 0; minesSpawned < nMines; minesSpawned++)
	{
		Vei2 spawnPos;
		do
		{
			spawnPos = { xDist(rng), yDist(rng) };
		} while (CellAt({ xDist(rng), yDist(rng) }).HasMine());
		CellAt(spawnPos).SpawnMine();
	}
}

Minefield::Cell& Minefield::CellAt(const Vei2 gridPos)
{
	return field[gridPos.y * width + gridPos.x];
}

void Minefield::Draw(Graphics& gfx)
{
	const Vei2 screenStartPos = { gfx.ScreenWidth/2 - (width * SpriteCodex::tileSize / 2) , gfx.ScreenHeight/2 - (height * SpriteCodex::tileSize / 2) };
	gfx.DrawRect(screenStartPos.x, screenStartPos.y, screenStartPos.x + width * SpriteCodex::tileSize, screenStartPos.y + height * SpriteCodex::tileSize, SpriteCodex::baseColor);
	for (Vei2 gridPos = { 0,0 }; gridPos.y < height; gridPos.y++)
	{
		for (gridPos.x = 0; gridPos.x < width; gridPos.x++)
		{
			const Vei2 screenPos = screenStartPos + gridPos * SpriteCodex::tileSize;

			if (!isDead)
			{
				switch (CellAt(gridPos).GetStatus())
				{
				case Cell::Status::Hidden:
					SpriteCodex::DrawTileButton(screenPos, gfx);
					break;
				case Cell::Status::Revealed:
					if (CellAt(gridPos).HasMine())
					{
						isDead = true;
					}
					else
					{
						DrawCellByNumber(CellAt(gridPos).GetNearbyMines(), gfx, screenPos);
					}
					break;
				case Cell::Status::Flagged:
					SpriteCodex::DrawTileButton(screenPos, gfx);
					SpriteCodex::DrawTileFlag(screenPos, gfx);
					break;
				}
			}
			else
			{
				switch (CellAt(gridPos).GetStatus())
				{
				case Cell::Status::Hidden:
					if (CellAt(gridPos).HasMine())
					{
						SpriteCodex::DrawTile0(screenPos, gfx);
						SpriteCodex::DrawTileBomb(screenPos, gfx);
					}
					else
					{
						SpriteCodex::DrawTileButton(screenPos, gfx);
					}
					break;
				case Cell::Status::Revealed:
					if (CellAt(gridPos).HasMine())
					{
						SpriteCodex::DrawTile0(screenPos, gfx);
						SpriteCodex::DrawTileBombRed(screenPos, gfx);
					}
					else
					{
						SpriteCodex::DrawTile0(screenPos, gfx);
					}
					break;
				case Cell::Status::Flagged:
					SpriteCodex::DrawTile0(screenPos, gfx);
					if (CellAt(gridPos).HasMine())
					{
						SpriteCodex::DrawTileBomb(screenPos, gfx);
						SpriteCodex::DrawTileFlag(screenPos, gfx);
					}
					else
					{
						SpriteCodex::DrawTileFlag(screenPos, gfx);
						SpriteCodex::DrawTileCross(screenPos, gfx);
					}
					break;
				}
			}
		}
	}
}

Vei2 Minefield::ScreenToGrid(const Vei2& screenPos, Graphics& gfx)
{
	const Vei2 screenStartPos = { gfx.ScreenWidth / 2 - (width * SpriteCodex::tileSize / 2) , gfx.ScreenHeight / 2 - (height * SpriteCodex::tileSize / 2) };
	return (screenPos - screenStartPos) / SpriteCodex::tileSize;
}

void Minefield::RevealOnClick(const Vei2& screenPos, Graphics& gfx)
{
	Vei2 gridPos = ScreenToGrid(screenPos, gfx);
	if (gridPos.x < width && gridPos.x >= 0 && gridPos.y < height && gridPos.y >= 0)
	{
		Cell& cell = CellAt(gridPos);
		assert(gridPos.x >= 0 && gridPos.x < width&& gridPos.y >= 0 && gridPos.y < height);
		cell.Reveal();
	}
}

void Minefield::ToggleFlagOnClick(const Vei2& screenPos, Graphics& gfx)
{
	Vei2 gridPos = ScreenToGrid(screenPos, gfx);
	if (gridPos.x < width && gridPos.x >= 0 && gridPos.y < height && gridPos.y >= 0)
	{
		Cell& cell = CellAt(gridPos);
		assert(gridPos.x >= 0 && gridPos.x < width&& gridPos.y >= 0 && gridPos.y < height);
		cell.ToggleFlag();
	}
}

void Minefield::CountNearbyMines()
{
	for (Vei2 gridPos = { 0,0 }; gridPos.y < height; gridPos.y++)
	{
		for (gridPos.x = 0; gridPos.x < width; gridPos.x++)
		{
			int nearbyMines = 0;

			const int startX = gridPos.x - 1;
			const int endX = gridPos.x + 1;
			const int startY = gridPos.y - 1;
			const int endY = gridPos.y + 1;

			for (int x = std::max(startX, 0); x <= std::min(endX, width-1); x++)
			{
				for (int y = std::max(startY, 0); y <= std::min(endY, height-1); y++)
				{
					if (CellAt({ x,y }).HasMine())
					{
						nearbyMines++;
					}
				}
			}
			CellAt(gridPos).SetNearbyMines(nearbyMines);
		}
	}
}

void Minefield::DrawCellByNumber(int number, Graphics& gfx, const Vei2& screenPos)
{
	switch (number)
	{
	case 0:
		SpriteCodex::DrawTile0(screenPos, gfx);
		break;
	case 1:
		SpriteCodex::DrawTile1(screenPos, gfx);
		break;
	case 2:
		SpriteCodex::DrawTile2(screenPos, gfx);
		break;
	case 3:
		SpriteCodex::DrawTile3(screenPos, gfx);
		break;
	case 4:
		SpriteCodex::DrawTile4(screenPos, gfx);
		break;
	case 5:
		SpriteCodex::DrawTile5(screenPos, gfx);
		break;
	case 6:
		SpriteCodex::DrawTile6(screenPos, gfx);
		break;
	case 7:
		SpriteCodex::DrawTile7(screenPos, gfx);
		break;
	case 8:
		SpriteCodex::DrawTile8(screenPos, gfx);
		break;
	}
}

void Minefield::RevealSafeCells()
{
	for (Vei2 gridPos = { 0,0 }; gridPos.y < height; gridPos.y++)
	{
		for (gridPos.x = 0; gridPos.x < width; gridPos.x++)
		{
			bool safeToReveal = false;
			const int startX = gridPos.x - 1;
			const int endX = gridPos.x + 1;
			const int startY = gridPos.y - 1;
			const int endY = gridPos.y + 1;

			for (int x = std::max(startX, 0); x <= std::min(endX, width - 1); x++)
			{
				for (int y = std::max(startY, 0); y <= std::min(endY, height - 1); y++)
				{
					if ((CellAt({ x, y }).HasMine() && !CellAt(gridPos).HasMine()) || !CellAt({ x, y }).HasMine() && !CellAt(gridPos).HasMine())
					{
						CellAt(gridPos).Reveal();
						break;
					}
				}
			}
		}
	}
}

Minefield::GameStatus Minefield::CheckWinStatus()
{
	int checkedCells = 0;
	for(Cell c : field)
	{
		if ( (!c.HasMine() && c.IsRevealed()) || ( c.HasMine() && c.IsFlagged() ) )
		{
			checkedCells++;
		}
	}
	if (checkedCells == width * height)
	{
		return GameStatus::Win;
	}
	else if (isDead)
	{
		return GameStatus::Lost;
	}
	else
	{
		return GameStatus::Started;
	}
}

void Minefield::Cell::Reveal()
{
	if (!IsRevealed() && !IsFlagged())
	{
		status = Status::Revealed;
	}
}

void Minefield::Cell::ToggleFlag()
{
	if (!IsRevealed())
	{
		if (!IsFlagged())
		{
			status = Status::Flagged;
		}
		else
		{
			status = Status::Hidden;
		}
	}
}

bool Minefield::Cell::IsRevealed()
{
	return status == Status::Revealed;
}

bool Minefield::Cell::IsFlagged()
{
	return status == Status::Flagged;
}

void Minefield::Cell::SpawnMine()
{
	if (!hasMine)
	{
		hasMine = true;
	}
}

bool Minefield::Cell::HasMine()
{
	return hasMine;
}

Minefield::Cell::Status& Minefield::Cell::GetStatus()
{
	return status;
}

void Minefield::Cell::SetNearbyMines(int in_nearbyMines)
{
	nearbyMines = in_nearbyMines;
}

int Minefield::Cell::GetNearbyMines()
{
	return nearbyMines;
}
