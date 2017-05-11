/*
 *
 * events_test
 * ledger-core
 *
 * Created by Pierre Pollastri on 27/04/2017.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Ledger
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <gtest/gtest.h>
#include <src/events/EventBus.hpp>
#include <async/QtThreadDispatcher.hpp>
#include <src/events/LambdaEventReceiver.hpp>
#include <src/events/Event.hpp>
#include <src/collections/DynamicObject.hpp>

using namespace ledger::core;
using namespace ledger::qt;

TEST(Events, SimpleCase) {
    auto dispatcher = std::make_shared<QtThreadDispatcher>();
    auto eventPublisher = std::make_shared<EventPublisher>(dispatcher->getSerialExecutionContext("worker"));

    eventPublisher->getEventBus()->subscribe(dispatcher->getMainExecutionContext(), make_receiver([&] (const std::shared_ptr<api::Event>& event) {
        EXPECT_EQ(event->getCode(), api::EventCode::SYNCHRONIZATION_START);
        dispatcher->stop();
    }));

    eventPublisher->post(make_event(api::EventCode::SYNCHRONIZATION_START, nullptr));
    dispatcher->waitUntilStopped();
}

TEST(Events, SimpleCaseWithPayload) {
    auto dispatcher = std::make_shared<QtThreadDispatcher>();
    auto eventPublisher = std::make_shared<EventPublisher>(dispatcher->getSerialExecutionContext("worker"));

    eventPublisher->getEventBus()->subscribe(dispatcher->getMainExecutionContext(), make_receiver([&] (const std::shared_ptr<api::Event>& event) {
        EXPECT_EQ(event->getCode(), api::EventCode::SYNCHRONIZATION_START);
        EXPECT_EQ(event->getPayload()->getString("hello"), Option<std::string>("world").toOptional());
        auto before = event->getPayload()->dump();
        event->getPayload()->putString("world", "hello"); // Payload should be read-only
        EXPECT_EQ(before, event->getPayload()->dump());
        dispatcher->stop();
    }));

    auto payload = std::make_shared<DynamicObject>();
    payload->putString("hello", "world");
    eventPublisher->post(make_event(api::EventCode::SYNCHRONIZATION_START, payload));
    dispatcher->waitUntilStopped();
}

TEST(Events, StickyEvent) {
    auto dispatcher = std::make_shared<QtThreadDispatcher>();
    auto eventPublisher = std::make_shared<EventPublisher>(dispatcher->getSerialExecutionContext("worker"));

    eventPublisher->postSticky(make_event(api::EventCode::SYNCHRONIZATION_START, nullptr), 0);
    eventPublisher->getEventBus()->subscribe(dispatcher->getMainExecutionContext(), make_receiver([&] (const std::shared_ptr<api::Event>& event) {
        EXPECT_EQ(event->getCode(), api::EventCode::SYNCHRONIZATION_START);
        dispatcher->stop();
    }));

    dispatcher->waitUntilStopped();
}

TEST(Events, Relay) {
    auto dispatcher = std::make_shared<QtThreadDispatcher>();
    auto eventPublisher = std::make_shared<EventPublisher>(dispatcher->getSerialExecutionContext("worker"));
    auto relay = std::make_shared<EventPublisher>(dispatcher->getSerialExecutionContext("worker"));

    relay->relay(eventPublisher->getEventBus());
    relay->getEventBus()->subscribe(dispatcher->getMainExecutionContext(), make_receiver([&] (const std::shared_ptr<api::Event>& event) {
        EXPECT_EQ(event->getCode(), api::EventCode::SYNCHRONIZATION_START);
        dispatcher->stop();
    }));

    dispatcher->getMainExecutionContext()->delay(ledger::qt::make_runnable([=] () {
        eventPublisher->post(make_event(api::EventCode::SYNCHRONIZATION_START, nullptr));
    }), 20);
    dispatcher->waitUntilStopped();
}