Original Repository:
Jason Ma (jasonma312)
Pei Ma (pmashi)
https://github.com/jasonma312/final-project.git

Compilation Steps: 
cd to the final-project folder 
run this command in the terminal: 
g++ sweep_output/main.cpp src/*.cpp -Iinclude -std=c++17 -o stocksim


the .exe file will be in the root folder 

Execution Steps: 
run this command in the terminal: 
./stocksim

How to use the program: 

1. Log in with any 'user name' and 'user ID'. The log in feature is not proper, so any input will work. 

2. Then, you should see the menu, which gives you 16 options, (0-15). 

3. First, you should load CSV data by entering the choice '1'. 

To load CSV data, you will be prompted in the following order: 
Ticker, Name, CSV Path, Type (STOCK or ETF)

If you type 'ETF' when requested for the type, you will be prompted for: 
Expense Ratio

In regards to what you should input, 
For ticker, a stock ticker is a string of capital letters. The available are: 
SPX 
