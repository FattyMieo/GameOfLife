#ifndef _TRIANGLE_DEMO_H
#define _TRIANGLE_DEMO_H

#define TEXTURE_COUNT 4

#include <vector>
#include "lodepng.h"
#include "demo_base.h"

GLuint mTextureID[TEXTURE_COUNT];

void loadPNG(const char* path, GLuint textureID)
{
	//Load file and decode image
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, path);

	//If there's an error, display it
	if (error != 0)
	{
		std::cout << "png load error :" << error << ": " << lodepng_error_text(error) << std::endl;
		return;
	}

	glBindTexture(GL_TEXTURE_2D, textureID);

	//Repeat the texture after exceeding 1.0f
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // apply texture wrapping along horizontal part
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // apply texture wrapping along vertical part

	//Uses last pixel after exceeding 1.0f
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // apply texture wrapping along horizontal part
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); // apply texture wrapping along vertical part

	// bilinear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // near filtering (For when texture needs to scale...)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // far filtering (For when texture needs to scale...)

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // near filtering (For when texture needs to scale...)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // far filtering (For when texture needs to scale...)

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
}

class Button
{
public:
	int x, y, width, height;
	GLuint textureID = 0;

	void draw()
	{
		glEnable(GL_TEXTURE_2D); //Enable texturing
		glBindTexture(GL_TEXTURE_2D, textureID); //Bind Texture

		GLfloat btnVertices[18] =
		{
			x			, y				, 0.0f,
			x			, y + height	, 0.0f,
			x + width	, y				, 0.0f,

			x + width	, y + height	, 0.0f,
			x + width	, y				, 0.0f,
			x			, y + height	, 0.0f
		};

		GLubyte btnColors[18] =
		{
			255, 255, 255,
			255, 255, 255,
			255, 255, 255,

			255, 255, 255,
			255, 255, 255,
			255, 255, 255
		};

		GLfloat btnTexCoords[] =
		{
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 0.0f,

			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 1.0f
		};

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, btnVertices);
		glColorPointer(3, GL_UNSIGNED_BYTE, 0, btnColors);
		glTexCoordPointer(2, GL_FLOAT, 0, btnTexCoords);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glDisable(GL_TEXTURE_2D); //Disable texturing

	}

	bool isWithinRange(int x, int y)
	{
		if
		(
			x >= this->x && x <= this->x + width &&
			y >= this->y && y <= this->y + height
		)
			return true;

		return false;
	}

	void init(int x, int y, int width, int height, GLuint textureID) // Position counts from top-left
	{
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->textureID = textureID;
	}
};

class TriangleDemo : public DemoBase
{
public:
	bool isSimulated = false;
	bool isInitializing = true;

	//Time
	clock_t waitMS = 30;
	clock_t startTime;
	clock_t endTime;

	GLfloat* boardVertices;
	GLubyte* boardColors;
	bool* boardOfLife;
	int windowWidth;
	int windowHeight;
	int boardWidth;
	int boardHeight;
	int cellSize;
	int boardOffsetX;
	int boardOffsetY;
	int preset = 0;

	//Mouse Input Cache
	int mouseX = 0;
	int mouseY = 0;
	bool isHoldingMouse = false;
	int mouseButton = -1;

	//Buttons
	int btnWidth = 120;
	int btnHeight = 40;
	Button playBtn;
	Button clearBtn;
	Button sizeBtn;
	Button speedBtn;

	GLubyte quadBlackColor[18] =
	{
		25, 25, 25,
		25, 25, 25,
		25, 25, 25,

		25, 25, 25,
		25, 25, 25,
		25, 25, 25
	};

	GLubyte quadWhiteColor[18] =
	{
		255, 255, 255,
		255, 255, 255,
		255, 255, 255,

		255, 255, 255,
		255, 255, 255,
		255, 255, 255
	};

	void initBoard()
	{
		isInitializing = true;

		isSimulated = false;

		boardOffsetX = (windowWidth - (boardWidth * cellSize)) / 2;

		int cellsNeeded = boardWidth * boardHeight;
		int verticesNeeded = cellsNeeded * 18;

		//Setup values
		boardVertices = new GLfloat[verticesNeeded];
		boardColors = new GLubyte[verticesNeeded];
		boardOfLife = new bool[cellsNeeded];

		for (int i = 0; i < boardHeight; i++)
		{
			for (int j = 0; j < boardWidth; j++)
			{
				initializeCell(j, i);
			}
		}

		isInitializing = false;
	}

	void init(int windowWidth, int windowHeight)
	{
		// To support alpha channel
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_ALPHA_TEST);

		glGenTextures(TEXTURE_COUNT, mTextureID);
		loadPNG("../media/Textures/button_play-pause.png", mTextureID[0]);
		loadPNG("../media/Textures/button_clear-map.png", mTextureID[1]);
		loadPNG("../media/Textures/button_map-size.png", mTextureID[2]);
		loadPNG("../media/Textures/button_speed.png", mTextureID[3]);

		playBtn.init((windowWidth / 5 * 1) - (btnWidth / 2), windowHeight - btnHeight, btnWidth, btnHeight, mTextureID[0]);
		clearBtn.init((windowWidth / 5 * 2) - (btnWidth / 2), windowHeight - btnHeight, btnWidth, btnHeight, mTextureID[1]);
		sizeBtn.init((windowWidth / 5 * 3) - (btnWidth / 2), windowHeight - btnHeight, btnWidth, btnHeight, mTextureID[2]);
		speedBtn.init((windowWidth / 5 * 4) - (btnWidth / 2), windowHeight - btnHeight, btnWidth, btnHeight, mTextureID[3]);

		this->windowWidth = windowWidth;
		this->windowHeight = windowHeight;
		//this->boardWidth = windowWidth / cellSize;
		//this->boardHeight = windowHeight / cellSize;
		boardWidth = 30;
		boardHeight = 30;
		cellSize = 12;
		boardOffsetY = 80;

		initBoard();
	}

	void deinit()
	{
	}

	void initializeCell(int x, int y, bool colorOnly = false)
	{
		int vertX = x * cellSize;
		int vertY = y * cellSize;

		/*
		int newVertices[] =
		{
		vertX + vertY							,	vertX + vertY + cellSize,
		vertX + vertY + (windowWidth * cellSize),	vertX + vertY + cellSize + (windowWidth * cellSize)
		};
		*/

		int firstCell = (x + (y * boardWidth));
		int firstVert = firstCell * 18;

		boardOfLife[firstCell] = false;

		if (!colorOnly)
		{
			GLfloat quadVertices[18] =
			{
				boardOffsetX + vertX				, boardOffsetY + vertY				, 0.0f,
				boardOffsetX + vertX				, boardOffsetY + vertY + cellSize	, 0.0f,
				boardOffsetX + vertX + cellSize		, boardOffsetY + vertY				, 0.0f,

				boardOffsetX + vertX + cellSize		, boardOffsetY + vertY + cellSize	, 0.0f,
				boardOffsetX + vertX + cellSize		, boardOffsetY + vertY				, 0.0f,
				boardOffsetX + vertX				, boardOffsetY + vertY + cellSize	, 0.0f
			};

			for (int k = 0; k < 18; k++)
			{
				boardVertices[firstVert + k] = quadVertices[k];
				boardColors[firstVert + k] = quadBlackColor[k];
			}
		}
		else
		{
			boardOfLife[firstCell] = false;

			for (int k = 0; k < 18; k++)
			{
				boardColors[firstVert + k] = quadBlackColor[k];
			}
		}
	}

	bool getCellState(bool* board, int x, int y)
	{
		int firstCell = (x + (y * boardWidth));
		if (firstCell >= boardWidth * boardHeight) return false;
		return board[firstCell];
	}

	void updateCell(int x, int y)
	{
		int firstCell = (x + (y * boardWidth));
		if (firstCell >= boardWidth * boardHeight) return;
		int firstVert = firstCell * 18;

		for (int k = 0; k < 18; k++)
		{
			if (boardOfLife[firstCell])
				boardColors[firstVert + k] = quadWhiteColor[k];
			else
				boardColors[firstVert + k] = quadBlackColor[k];
		}
	}

	void updateCell(int x, int y, bool isMouseInput)
	{
		if (!isMouseInput) return updateCell(x, y);

		int cellX = (x - boardOffsetX) / cellSize;
		if (cellX < 0) return;
		if (cellX >= boardWidth) return;
		int cellY = (y - boardOffsetY) / cellSize;
		if (cellY < 0) return;
		if (cellY >= boardHeight) return;

		return updateCell(cellX, cellY);
	}

	void setCellState(bool* board, int x, int y, bool state)
	{
		int firstCell = (x + (y * boardWidth));
		if (firstCell >= boardWidth * boardHeight) return;
		int firstVert = firstCell * 18;

		board[firstCell] = state;
	}

	void setCellState(bool* board, int x, int y, bool state, bool isMouseInput)
	{
		if (!isMouseInput) return setCellState(board, x, y, state);

		int cellX = (x - boardOffsetX) / cellSize;
		if (cellX < 0) return;
		if (cellX >= boardWidth) return;
		int cellY = (y - boardOffsetY) / cellSize;
		if (cellY < 0) return;
		if (cellY >= boardHeight) return;

		return setCellState(board, cellX, cellY, state);
	}

	//Main Logic Function
	void checkBoard()
	{
		bool* newBoardOfLife = new bool[boardWidth * boardHeight];

		for (int i = 0; i < boardHeight; i++)
		{
			for (int j = 0; j < boardWidth; j++)
			{
				bool alive = getCellState(boardOfLife, j, i);

				int neighbours = 0;
				int cellLR = 0;

				//Neighbour Counting
				if (j > 0) //Has Left Cell
				{
					cellLR--;
					if (getCellState(boardOfLife, j - 1, i)) neighbours++;
				}
				if (j < boardWidth - 1) // Has Right Cell
				{
					cellLR++;
					if (getCellState(boardOfLife, j + 1, i)) neighbours++;
				}

				if (i > 0) //Has Top Cell
				{
					if (getCellState(boardOfLife, j, i - 1)) neighbours++;

					if (cellLR <= 0) //Has Left Cell
					{
						if (getCellState(boardOfLife, j - 1, i - 1)) neighbours++;
					}

					if (cellLR >= 0) //Has Right Cell
					{
						if (getCellState(boardOfLife, j + 1, i - 1)) neighbours++;
					}
				}
				if (i < boardHeight - 1) // Has Bottom Cell
				{
					if (getCellState(boardOfLife, j, i + 1)) neighbours++;

					if (cellLR <= 0) //Has Left Cell
					{
						if (getCellState(boardOfLife, j - 1, i + 1)) neighbours++;
					}

					if (cellLR >= 0) //Has Right Cell
					{
						if (getCellState(boardOfLife, j + 1, i + 1)) neighbours++;
					}
				}

				//Live Decision
				if (alive)
				{
					if (neighbours < 2 || neighbours > 3) alive = false;
				}
				else
				{
					if (neighbours == 3) alive = true;
				}

				setCellState(newBoardOfLife, j, i, alive);
			}
		}

		for (int i = 0; i < boardHeight; i++)
		{
			for (int j = 0; j < boardWidth; j++)
			{
				bool oldState = getCellState(boardOfLife, j, i);
				bool newState = getCellState(newBoardOfLife, j, i);

				if (oldState != newState)
				{
					setCellState(boardOfLife, j, i, newState);
					updateCell(j, i);
				}
			}
		}
	}

	void draw()
	{
		/*
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_TRIANGLES);								// Drawing Using Triangles
			glVertex3f( 0.0f, 0.0f, 0.0f);					// Top
			glVertex3f( 300.0f, 0.0f, 0.0f);					// Bottom Left
			glVertex3f( 0.0f, 300.0f, 0.0f);					// Bottom Right
		glEnd();										// Finished Drawing The Triangle
		*/

		if (isSimulated)
		{
			startTime = clock();
			endTime = startTime + waitMS;

			checkBoard();

			while (clock() < endTime && isSimulated);
		}

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, boardVertices);
		glColorPointer(3, GL_UNSIGNED_BYTE, 0, boardColors);
		glDrawArrays(GL_TRIANGLES, 0, boardHeight * boardWidth * 6);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);

		playBtn.draw();
		clearBtn.draw();
		sizeBtn.draw();
		speedBtn.draw();
	}

	void onMouseMove(double x, double y)
	{
		if (isInitializing) return;
		//Debug
		//std::cout << "mouse pos: " << x << ", " << y << std::endl;

		mouseX = x;
		mouseY = y;

		if (isHoldingMouse)
		{
			if (mouseButton == 0)
				setCellState(boardOfLife, mouseX, mouseY, true, true);
			else if (mouseButton == 1)
				setCellState(boardOfLife, mouseX, mouseY, false, true);

			updateCell(mouseX, mouseY, true);
		}
	}

	void onMouseButton(int button, int action)
	{
		if (isInitializing) return;

		//Debug
		//std::cout << "mouse button: " << button << ", " << action << std::endl;

		if (action == GLFW_PRESS)
		{
			isHoldingMouse = true;

			//One-time click
			if (mouseButton == 0)
				setCellState(boardOfLife, mouseX, mouseY, true, true);
			else if (mouseButton == 1)
				setCellState(boardOfLife, mouseX, mouseY, false, true);

			updateCell(mouseX, mouseY, true);

			if (playBtn.isWithinRange(mouseX, mouseY))
			{
				isSimulated = !isSimulated;
			}
			else if (clearBtn.isWithinRange(mouseX, mouseY))
			{
				isSimulated = false;

				isInitializing = true;

				for (int i = 0; i < boardHeight; i++)
				{
					for (int j = 0; j < boardWidth; j++)
					{
						initializeCell(j, i, true);
					}
				}

				isInitializing = false;
			}
			else if (sizeBtn.isWithinRange(mouseX, mouseY))
			{
				++preset;
				switch (preset)
				{
				case 1:
					boardWidth = 80;
					boardHeight = 50;
					cellSize = 8;
					boardOffsetY = 40;
					break;
				case 2:
					boardWidth = 200;
					boardHeight = 100;
					cellSize = 4;
					boardOffsetY = 40;
					break;
				case 3:
					boardWidth = 500;
					boardHeight = 250;
					cellSize = 2;
					boardOffsetY = 20;
					break;
				case 4:
					boardWidth = 750;
					boardHeight = 400;
					boardOffsetY = 20;
					cellSize = 2;
					break;
				case 5:
					boardWidth = 900;
					boardHeight = 500;
					boardOffsetY = 10;
					cellSize = 2;
					break;
				case 6:
					boardWidth = 960;
					boardHeight = 521;
					boardOffsetY = 0;
					cellSize = 2;
					break;
				default:
					preset = 0;
					boardWidth = 30;
					boardHeight = 30;
					boardOffsetY = 80;
					cellSize = 12;
				}

				isSimulated = false;

				initBoard();
			}
			else if (speedBtn.isWithinRange(mouseX, mouseY))
			{
				waitMS += 170;
				if (waitMS > 200)
					waitMS = 30;
			}
		}
		else if (action == GLFW_RELEASE)
		{
			isHoldingMouse = false;
		}

		mouseButton = button;
	}

	bool hasPressedSpacebar = false;

	void onKeyButton(int button, int action)
	{
		if (isInitializing) return;

		if (button == GLFW_KEY_SPACE)
		{
			if (action == GLFW_PRESS)
			{
				if (!hasPressedSpacebar)
				{
					isSimulated = !isSimulated;
					hasPressedSpacebar = true;
				}
			}
			else if (action == GLFW_RELEASE)
			{
				hasPressedSpacebar = false;
			}
		}
	}
};

#endif
