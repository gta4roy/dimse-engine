# Alternative to scripts/build.sh — still requires a prebuilt DCMTK at $DCMTK_LIB.
# Prefer ./scripts/build.sh (CMake) for day-to-day use.

CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2
SRC_DIR   = src
INC_DIR   = include
BUILD_DIR = build
TARGET    = $(BUILD_DIR)/dimse-engine

SRCS = $(SRC_DIR)/main.cpp $(SRC_DIR)/cstore_scu.cpp
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

ifeq ($(DCMTK_LIB),)
  $(error DCMTK_LIB is not set. export DCMTK_LIB=/path/to/dcmtk/install)
endif

DCMTK_INC = $(DCMTK_LIB)/include
DCMTK_LIBDIR = $(DCMTK_LIB)/lib

CPPFLAGS += -I$(INC_DIR) -I$(DCMTK_INC) -DHAVE_CONFIG_H
LDFLAGS  += -L$(DCMTK_LIBDIR) -Wl,-rpath,$(DCMTK_LIBDIR)
LDLIBS   += -ldcmnet -ldcmdata -loflog -lofstd -lpthread

.PHONY: all clean

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -rf $(BUILD_DIR)
