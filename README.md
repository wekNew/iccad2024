#刪除資料夾
rm -rf iccad2024/
#git clone檔案
git clone https://github.com/wekNew/iccad2024.git

#建立makefile檔
touch makefile

#把以下指令打到makefile檔中(只需要確保SRCS等號後，有所需的所有.cpp檔，所以有新增要自己補充上去)
---------------------------------------------------------------------------------------------------------------------------------------
CXX = g++
CXXFLAGS = -Wall -O2 -std=c++11 -fopenmp
LDFLAGS = -fopenmp
TARGET = my_project 
SRCS = main.cpp cell.cpp Cluster.cpp ClustersBuilder.cpp meanShift.cpp netlist.cpp partition.cpp pin.cpp Point.cpp table.cpp legalize.cpp tetris.cpp
OBJS = $(SRCS:.cpp=.o)
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
run: $(TARGET)
	OMP_NUM_THREADS=8 ./$(TARGET)
clean:
	rm -f $(TARGET) $(OBJS)
------------------------------------------------------------------------------------------------------------------------------------------

#產生執行檔
make


-----------------------------------------------------接著就可以開screen執行了-------------------------------------------------
#開啟screen
screen -S mysession

#在裡面執行
./my_project

#按下 Ctrl + A 然後按 D 鍵(分離螢幕，程式會繼續執行)

#重新連接到 screen 會話以檢查結果
screen -r mysession

#查看有哪些screen
screen -ls

#刪除並強制停止screen
screen -X -S mysession quit
