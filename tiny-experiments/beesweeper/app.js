document.addEventListener('DOMContentLoaded', () => {
    // grid & square dimensions
    const gridSize = 400
    const width = 10
    const squareSize = gridSize / width >> 0
    const squaresCount = width * width
    const beesCount = 10

    const grid = document.querySelector('.grid')
    grid.style.width = gridSize + "px"
    grid.style.height = gridSize + "px"
    const leftBeesText = document.querySelector("[leftbees]")
    let leftBees = beesCount
    leftBeesText.textContent = leftBees
    let squares = []

    let isGameOver = false
    let leftUnrevealed = squaresCount - beesCount

    const bees = Array(beesCount).fill('bee')
    const numbers = Array(squaresCount - beesCount).fill('number')
    const cellClasses = bees.concat(numbers)
    const shuffledCellClasses = cellClasses.sort(() => Math.random() - 0.5)

    const surroundings = [
        [-1, -1], [-1, 0], [-1, 1],
        [0, -1], /*[0, 0],*/[0, 1],
        [1, -1], [1, 0], [1, 1]]

    function createGrid() {
        for (i = 0; i < squaresCount; i++) {
            const square = document.createElement('div')
            square.setAttribute('id', i)
            square.style.height = squareSize + "px"
            square.style.width = squareSize + "px"
            square.classList.add(shuffledCellClasses[i])
            grid.appendChild(square)
            squares.push(square)

            square.addEventListener('click', (e) =>
                click(square)
            )

            square.addEventListener('contextmenu', (e) => {
                e.preventDefault()
                markBee(square)
            })
        }

        for (let i = 0; i < width; i++) {
            for (let j = 0; j < width; j++) {
                let surroundingBeesCount = 0
                let currentSquare = squares[i * width + j]
                if (!currentSquare.classList.contains('bee')) {
                    for (let s = 0; s < surroundings.length; s++) {
                        const ii = i + surroundings[s][0]
                        const jj = j + surroundings[s][1]
                        if (insideGrid(ii, jj) &&
                            squares[ii * width + jj].classList.contains('bee')) {
                            surroundingBeesCount++
                        }
                    }
                    currentSquare.setAttribute('beescount', surroundingBeesCount)
                }
            }
        }
    }

    function insideGrid(i, j) {
        return i >= 0 && i < width && j >= 0 && j < width
    }

    function click(square) {
        if (!isGameOver) {
            if (square.classList.contains("revealed") ||
                square.classList.contains("flower")) {
                //
            }
            else if (square.classList.contains('bee')) {
                gameOver()
            }
            else {
                const surroundingBeesCount = square.getAttribute("beescount")
                square.classList.add("revealed")
                leftUnrevealed -= 1
                if (surroundingBeesCount == 0) {
                    checkSurroundingSquares(square)
                }
                else {
                    square.innerHTML = surroundingBeesCount
                }
                checkWin()
            }
        }
        return
    }

    function checkSurroundingSquares(square) {
        const i = square.id / width >> 0
        const j = square.id % width
        for (let s = 0; s < surroundings.length; s++) {
            let ii = i + surroundings[s][0]
            let jj = j + surroundings[s][1]

            if (insideGrid(ii, jj)) {
                squareToCheck = squares[ii * width + jj]
                if (!square.classList.contains('bee')) {
                    click(squareToCheck)
                }
            }
        }
    }

    function gameOver() {
        isGameOver = true
        squares.forEach(square => {
            if (square.classList.contains("bee")) {
                square.innerHTML = "🐝"
                square.style.background = "chocolate"
            }
        })
        setTimeout(function () {
            alert("Game is over! You lose! 🐝")
        }, 1)

        return
    }

    function checkWin() {
        if (leftUnrevealed == 0) {
            isGameOver = true
            setTimeout(function () {
                alert("Game is over! You win! 🎉")
            }, 1)
        }
        return
    }

    function markBee(square) {
        if (!square.classList.contains("revealed")) {
            if (!square.classList.contains("flower")) {
                square.classList.add("flower")
                square.innerHTML = '🌸'
                leftBees -= 1
                leftBeesText.textContent = leftBees
            } else {
                square.classList.remove("flower")
                square.innerHTML = ''
                leftBees += 1
                leftBeesText.textContent = leftBees
            }
        }
        return
    }

    createGrid()
})