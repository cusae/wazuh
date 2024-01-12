/*
 * Wazuh content manager - Component Tests
 * Copyright (C) 2015, Wazuh Inc.
 * July 26, 2023.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */

#include "action_test.hpp"
#include "action.hpp"
#include "actionOrchestrator.hpp"
#include "gtest/gtest.h"
#include <chrono>
#include <filesystem>
#include <memory>
#include <string>
#include <thread>

/*
 * @brief Tests the instantiation of the Action class
 */
TEST_F(ActionTest, TestInstantiation)
{
    const auto& topicName {m_parameters.at("topicName").get_ref<const std::string&>()};
    const auto& outputFolder {m_parameters.at("configData").at("outputFolder").get_ref<const std::string&>()};

    EXPECT_NO_THROW(std::make_shared<Action>(m_spRouterProvider, topicName, m_parameters));

    EXPECT_TRUE(std::filesystem::exists(outputFolder));
}

/*
 * @brief Tests the instantiation of the Action class without configData
 */
TEST_F(ActionTest, TestInstantiationWhitoutConfigData)
{
    // creates a copy of `m_parameters` because it's used in `TearDown` method
    auto parameters = m_parameters;

    const auto& topicName {parameters.at("topicName").get_ref<const std::string&>()};

    parameters.erase("configData");

    EXPECT_THROW(std::make_shared<Action>(m_spRouterProvider, topicName, parameters), std::invalid_argument);
}

/*
 * @brief Tests the instantiation of the Action class and execution of startActionScheduler for raw data
 */
TEST_F(ActionTest, TestInstantiationAndStartActionSchedulerForRawData)
{
    const auto& topicName {m_parameters.at("topicName").get_ref<const std::string&>()};
    const auto& outputFolder {m_parameters.at("configData").at("outputFolder").get_ref<const std::string&>()};
    const auto& fileName {m_parameters.at("configData").at("contentFileName").get_ref<const std::string&>()};
    const auto& interval {m_parameters.at("interval").get_ref<const size_t&>()};
    const auto contentPath {outputFolder + "/" + CONTENTS_FOLDER + "/3-" + fileName};
    const auto downloadPath {outputFolder + "/" + DOWNLOAD_FOLDER + "/3-" + fileName};

    auto action {std::make_shared<Action>(m_spRouterProvider, topicName, m_parameters)};

    EXPECT_TRUE(std::filesystem::exists(outputFolder));

    EXPECT_NO_THROW(action->startActionScheduler(interval));

    std::this_thread::sleep_for(std::chrono::seconds(interval + 1));

    EXPECT_NO_THROW(action->stopActionScheduler());

    // This file shouldn't exist because it's a test for raw data
    EXPECT_FALSE(std::filesystem::exists(downloadPath));

    EXPECT_TRUE(std::filesystem::exists(contentPath));

    EXPECT_TRUE(std::filesystem::exists(outputFolder));
}

/*
 * @brief Tests the instantiation of the Action class and execution of startActionScheduler for compressed data with
 * deleteDownloadedContent enabled
 */
TEST_F(ActionTest, TestInstantiationAndStartActionSchedulerForRawDataWithDeleteDownloadedContentEnabled)
{
    m_parameters["configData"]["url"] = "http://localhost:4444/xz/consumers";
    m_parameters["configData"]["compressionType"] = "xz";
    m_parameters["configData"]["deleteDownloadedContent"] = true;

    const auto& topicName {m_parameters.at("topicName").get_ref<const std::string&>()};
    const auto& outputFolder {m_parameters.at("configData").at("outputFolder").get_ref<const std::string&>()};
    const auto& fileName {m_parameters.at("configData").at("contentFileName").get_ref<const std::string&>()};
    const auto& interval {m_parameters.at("interval").get_ref<const size_t&>()};
    const auto contentPath {outputFolder + "/" + CONTENTS_FOLDER + "/3-" + fileName};
    const auto downloadPath {outputFolder + "/" + DOWNLOAD_FOLDER + "/3-" + fileName};

    auto action {std::make_shared<Action>(m_spRouterProvider, topicName, m_parameters)};

    EXPECT_TRUE(std::filesystem::exists(outputFolder));

    EXPECT_NO_THROW(action->startActionScheduler(interval));

    std::this_thread::sleep_for(std::chrono::seconds(interval + 1));

    EXPECT_NO_THROW(action->stopActionScheduler());

    // This file shouldn't exist because deleteDownloadedContent is enabled
    EXPECT_FALSE(std::filesystem::exists(downloadPath));

    EXPECT_TRUE(std::filesystem::exists(contentPath));

    EXPECT_TRUE(std::filesystem::exists(outputFolder));
}

/*
 * @brief Tests the instantiation of the Action class and execution of startActionScheduler for
 * compressed data
 */
TEST_F(ActionTest, TestInstantiationAndStartActionSchedulerForCompressedData)
{
    m_parameters["configData"]["url"] = "http://localhost:4444/xz/consumers";
    m_parameters["configData"]["compressionType"] = "xz";

    const auto& topicName {m_parameters.at("topicName").get_ref<const std::string&>()};
    const auto& outputFolder {m_parameters.at("configData").at("outputFolder").get_ref<const std::string&>()};
    const auto& fileName {m_parameters.at("configData").at("contentFileName").get_ref<const std::string&>()};
    const auto& interval {m_parameters.at("interval").get_ref<const size_t&>()};
    const auto contentPath {outputFolder + "/" + CONTENTS_FOLDER + "/3-" + fileName};
    const auto downloadPath {outputFolder + "/" + DOWNLOAD_FOLDER + "/3-" + fileName};

    auto action {std::make_shared<Action>(m_spRouterProvider, topicName, m_parameters)};

    EXPECT_TRUE(std::filesystem::exists(outputFolder));

    EXPECT_NO_THROW(action->startActionScheduler(interval));

    std::this_thread::sleep_for(std::chrono::seconds(interval + 1));

    EXPECT_NO_THROW(action->stopActionScheduler());

    // This file should exist because deleteDownloadedContent is not enabled
    EXPECT_TRUE(std::filesystem::exists(downloadPath));

    EXPECT_TRUE(std::filesystem::exists(contentPath));

    EXPECT_TRUE(std::filesystem::exists(outputFolder));
}

/*
 * @brief Tests the instantiation of the Action class and execution of registerActionOnDemand for raw data
 */
TEST_F(ActionTest, TestInstantiationAndRegisterActionOnDemandForRawData)
{
    const auto& topicName {m_parameters.at("topicName").get_ref<const std::string&>()};
    const auto& outputFolder {m_parameters.at("configData").at("outputFolder").get_ref<const std::string&>()};

    m_parameters["ondemand"] = true;

    auto action {std::make_shared<Action>(m_spRouterProvider, topicName, m_parameters)};

    EXPECT_TRUE(std::filesystem::exists(outputFolder));

    EXPECT_NO_THROW(action->registerActionOnDemand());

    EXPECT_NO_THROW(action->unregisterActionOnDemand());

    EXPECT_NO_THROW(action->clearEndpoints());
}

/*
 * @brief Tests the instantiation of two Actions on demand with the same topicName
 */
TEST_F(ActionTest, TestInstantiationOfTwoActionsWithTheSameTopicName)
{
    const auto& topicName {m_parameters.at("topicName").get_ref<const std::string&>()};
    const auto& outputFolder {m_parameters.at("configData").at("outputFolder").get_ref<const std::string&>()};

    // Both actions can't use RocksDB.
    auto parametersWithoutDatabasePath = m_parameters;
    parametersWithoutDatabasePath.at("configData").erase("databasePath");

    m_parameters["ondemand"] = true;

    auto action1 {std::make_shared<Action>(m_spRouterProvider, topicName, m_parameters)};
    auto action2 {std::make_shared<Action>(m_spRouterProvider, topicName, parametersWithoutDatabasePath)};

    EXPECT_TRUE(std::filesystem::exists(outputFolder));

    EXPECT_NO_THROW(action1->registerActionOnDemand());
    EXPECT_THROW(action2->registerActionOnDemand(), std::runtime_error);

    EXPECT_NO_THROW(action1->unregisterActionOnDemand());

    EXPECT_NO_THROW(action1->clearEndpoints());
}

/*
 * @brief Tests the instantiation of the Action class and runActionOnDemand
 */
TEST_F(ActionTest, TestInstantiationAndRunActionOnDemand)
{
    m_parameters["ondemand"] = true;

    const auto& topicName {m_parameters.at("topicName").get_ref<const std::string&>()};
    const auto& outputFolder {m_parameters.at("configData").at("outputFolder").get_ref<const std::string&>()};
    const auto& fileName {m_parameters.at("configData").at("contentFileName").get_ref<const std::string&>()};
    const auto contentPath {outputFolder + "/" + CONTENTS_FOLDER + "/3-" + fileName};
    const auto downloadPath {outputFolder + "/" + DOWNLOAD_FOLDER + "/3-" + fileName};

    auto action {std::make_shared<Action>(m_spRouterProvider, topicName, m_parameters)};

    EXPECT_TRUE(std::filesystem::exists(outputFolder));

    EXPECT_NO_THROW(action->registerActionOnDemand());

    EXPECT_NO_THROW(action->runActionOnDemand());

    std::this_thread::sleep_for(std::chrono::seconds(1));

    EXPECT_NO_THROW(action->unregisterActionOnDemand());
    EXPECT_NO_THROW(action->clearEndpoints());

    // This file shouldn't exist because it's a test for raw data
    EXPECT_FALSE(std::filesystem::exists(downloadPath));

    EXPECT_TRUE(std::filesystem::exists(contentPath));

    EXPECT_TRUE(std::filesystem::exists(outputFolder));
}

/**
 * @brief Tests the on-start execution of the action.
 *
 */
TEST_F(ActionTest, ActionOnStartExecution)
{
    constexpr auto ACTION_INTERVAL {100};
    constexpr auto WAIT_TIME {1};

    const auto& topicName {m_parameters.at("topicName").get_ref<const std::string&>()};
    const auto& outputFolder {m_parameters.at("configData").at("outputFolder").get_ref<const std::string&>()};
    const auto& fileName {m_parameters.at("configData").at("contentFileName").get_ref<const std::string&>()};

    // Make the interval big enough to be sure the action is not triggered a second time.
    auto& interval {m_parameters.at("interval").get_ref<size_t&>()};
    interval = ACTION_INTERVAL;

    // Init action.
    auto action {std::make_shared<Action>(m_spRouterProvider, topicName, m_parameters)};

    // Check output folder existence.
    EXPECT_TRUE(std::filesystem::exists(outputFolder));

    // Start scheduling.
    EXPECT_NO_THROW(action->startActionScheduler(interval));

    // Wait just for a little time.
    std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME));

    // Stop scheduling.
    EXPECT_NO_THROW(action->stopActionScheduler());

    // Check that the download has been correctly made.
    const auto contentFilePath {outputFolder + "/" + CONTENTS_FOLDER + "/3-" + fileName};
    EXPECT_TRUE(std::filesystem::exists(contentFilePath));
}

/**
 * @brief Tests the correct catch of the exceptions thrown in the orchestration execution when the action is triggered
 * on demand.
 *
 */
TEST_F(ActionTest, OnDemandActionCatchException)
{
    // Set invalid URL, forcing the orchestration to fail in the download stage.
    m_parameters.at("configData").at("url") = "http://localhost:4444/invalid_url";

    // Init action.
    const auto& topicName {m_parameters.at("topicName").get_ref<const std::string&>()};
    auto action {std::make_shared<Action>(m_spRouterProvider, topicName, m_parameters)};

    // Trigger action. No exceptions are expected despite the error.
    ASSERT_NO_THROW(action->runActionOnDemand());

    // Check that no output files have been created.
    const std::filesystem::path outputFolder {
        m_parameters.at("configData").at("outputFolder").get_ref<const std::string&>()};
    EXPECT_TRUE(std::filesystem::is_empty(outputFolder / DOWNLOAD_FOLDER));
    EXPECT_TRUE(std::filesystem::is_empty(outputFolder / CONTENTS_FOLDER));
}

/**
 * @brief Tests the correct catch of the exceptions thrown in the orchestration execution when the action is triggered
 * by the scheduler.
 *
 */
TEST_F(ActionTest, ScheduledActionCatchException)
{
    // Set invalid URL, forcing the orchestration to fail in the download stage.
    m_parameters.at("configData").at("url") = "http://localhost:4444/invalid_url";

    // Init action.
    const auto& topicName {m_parameters.at("topicName").get_ref<const std::string&>()};
    auto action {std::make_shared<Action>(m_spRouterProvider, topicName, m_parameters)};

    // Start scheduling. First action execution.
    const auto& interval {m_parameters.at("interval").get_ref<size_t&>()};
    EXPECT_NO_THROW(action->startActionScheduler(interval));

    // Wait for second action execution.
    std::this_thread::sleep_for(std::chrono::seconds(interval + 1));
    EXPECT_NO_THROW(action->stopActionScheduler());

    // Check that no output files have been created.
    const std::filesystem::path outputFolder {
        m_parameters.at("configData").at("outputFolder").get_ref<const std::string&>()};
    EXPECT_TRUE(std::filesystem::is_empty(outputFolder / DOWNLOAD_FOLDER));
    EXPECT_TRUE(std::filesystem::is_empty(outputFolder / CONTENTS_FOLDER));
}

/**
 * @brief Test the on-demand action execution for an offset update process.
 *
 */
TEST_F(ActionTest, RunActionOnDemandOffsetUpdate)
{
    m_parameters["ondemand"] = true;
    const auto& topicName {m_parameters.at("topicName").get_ref<const std::string&>()};

    auto action {Action(m_spRouterProvider, topicName, m_parameters)};
    action.registerActionOnDemand();

    constexpr auto OFFSET {1000};
    ASSERT_NO_THROW(action.runActionOnDemand(OFFSET, ActionOrchestrator::UpdateType::OFFSET));

    action.unregisterActionOnDemand();
    action.clearEndpoints();
}
