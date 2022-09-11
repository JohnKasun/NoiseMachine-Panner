#pragma once

#include <gtest/gtest.h>

#include "DataFileIO.h"
#include "GTestUtil.h"
#include "PanBot.h"
#include "Synthesis.h"
#include "Vector.h"

class PanBotTestSuite : public ::testing::Test {
protected:
	void SetUp() override {
		mSampleRate = 44100;
		mPanBot.reset(new PanBot(mSampleRate));
	}
	void TearDown() override {
		mPanBot.reset();
		mInput.clear();
		mOutput[0].clear();
		mOutput[1].clear();
		mGround[0].clear();
		mGround[1].clear();
	}
	std::unique_ptr<PanBot> mPanBot;
	std::vector<float> mInput;
	std::array<std::vector<float>, 2> mOutput;
	std::array<std::vector<float>, 2> mGround;
	float mSampleRate;
};

TEST_F(PanBotTestSuite, Ones) {
	const int numSamples = 1000;
	float leftChannelGround[numSamples]{};
	float rightChannelGround[numSamples]{};
	CSynthesis::generateSine(leftChannelGround, 1, mSampleRate, numSamples, 0.5);
	CSynthesis::generateSine(rightChannelGround, 1, mSampleRate, numSamples, 0.5, M_PI);
	CVectorFloat::addC_I(leftChannelGround, 0.5, numSamples);
	CVectorFloat::addC_I(rightChannelGround, 0.5, numSamples);
	for (int i = 0; i < numSamples; i++) {
		mGround.at(0).push_back(leftChannelGround[i]);
		mGround.at(1).push_back(rightChannelGround[i]);

		auto output = mPanBot->process(1.0f);
		mOutput.at(0).push_back(std::get<0>(output));
		mOutput.at(1).push_back(std::get<1>(output));
	}
	GTestUtil::compare(mOutput.at(0), mGround.at(0));
	GTestUtil::compare(mOutput.at(1), mGround.at(1));
}

TEST_F(PanBotTestSuite, Zeros) {
	const int numSamples = 1000;
	for (int i = 0; i < numSamples; i++) {
		auto output = mPanBot->process(0.0f);
		mOutput.at(0).push_back(std::get<0>(output));
		mOutput.at(1).push_back(std::get<1>(output));
	}
	GTestUtil::compare(mOutput.at(0), mGround.at(0));
	GTestUtil::compare(mOutput.at(1), mGround.at(1));
}

TEST_F(PanBotTestSuite, Width) {
	auto freq = 5.0f;
	std::vector<float> widths{ 0, 25, 50, 75, 100 };
	for (auto width : widths) {
		SetUp();
		mPanBot->setSpeed(freq);
		mPanBot->setWidth(width);
		const int numSamples = 10000;
		float leftChannelGround[numSamples]{};
		float rightChannelGround[numSamples]{};
		CSynthesis::generateSine(leftChannelGround, freq, mSampleRate, numSamples, width / 200.0f);
		CSynthesis::generateSine(rightChannelGround, freq, mSampleRate, numSamples, width / 200.0f, M_PI);
		CVectorFloat::addC_I(leftChannelGround, 0.5, numSamples);
		CVectorFloat::addC_I(rightChannelGround, 0.5, numSamples);
		for (int i = 0; i < numSamples; i++) {
			mGround.at(0).push_back(leftChannelGround[i]);
			mGround.at(1).push_back(rightChannelGround[i]);

			auto output = mPanBot->process(1.0f);
			mOutput.at(0).push_back(std::get<0>(output));
			mOutput.at(1).push_back(std::get<1>(output));
		}
		GTestUtil::compare(mOutput.at(0), mGround.at(0));
		GTestUtil::compare(mOutput.at(1), mGround.at(1));
		TearDown();
	}
}