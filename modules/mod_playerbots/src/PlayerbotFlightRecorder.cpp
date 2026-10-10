/*
 * This file is part of the Legends of Azeroth Pandaria Project.
 */

#include "PlayerbotFlightRecorder.h"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <functional>
#include <mutex>
#include <sstream>
#include <thread>

#if defined(__linux__)
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "Map.h"
#include "ObjectGuid.h"
#include "Player.h"

namespace
{
constexpr char FlightLogPath[] = "/tmp/playerbot-flight-recorder.log";
constexpr char PreviousFlightLogPath[] =
    "/tmp/playerbot-flight-recorder.previous.log";
constexpr uint64 MaxFlightLogSize = 16ULL * 1024ULL * 1024ULL;

std::mutex FlightLogMutex;
std::atomic<uint64> FlightSequence{ 0 };

std::string CleanField(std::string value)
{
    for (char& character : value)
        if (character == '\n' || character == '\r' || character == '\t' ||
            static_cast<unsigned char>(character) < 0x20)
            character = ' ';

    // Keep each record comfortably below a single filesystem block so one
    // append syscall leaves either a complete final breadcrumb or no record.
    if (value.size() > 320)
        value.resize(320);
    return value;
}

#if defined(__linux__)
int OpenFlightLog(uint64& size)
{
    int const descriptor = open(FlightLogPath,
        O_WRONLY | O_CREAT | O_APPEND | O_CLOEXEC | O_NOFOLLOW, 0640);
    if (descriptor < 0)
        return -1;

    struct stat status = { };
    if (fstat(descriptor, &status) != 0 || !S_ISREG(status.st_mode))
    {
        close(descriptor);
        return -1;
    }

    size = status.st_size > 0 ? uint64(status.st_size) : 0;
    return descriptor;
}

void AppendRecord(std::string const& record)
{
    static int descriptor = -1;
    static uint64 size = 0;

    std::lock_guard<std::mutex> lock(FlightLogMutex);
    if (descriptor < 0)
        descriptor = OpenFlightLog(size);
    if (descriptor < 0)
        return;

    if (size + record.size() > MaxFlightLogSize)
    {
        close(descriptor);
        descriptor = -1;
        std::rename(FlightLogPath, PreviousFlightLogPath);
        descriptor = OpenFlightLog(size);
        if (descriptor < 0)
            return;
    }

    char const* data = record.data();
    size_t remaining = record.size();
    while (remaining)
    {
        ssize_t const written = write(descriptor, data, remaining);
        if (written < 0)
        {
            if (errno == EINTR)
                continue;
            close(descriptor);
            descriptor = -1;
            return;
        }
        if (!written)
            return;
        data += written;
        remaining -= size_t(written);
        size += uint64(written);
    }
}
#else
void AppendRecord(std::string const& record)
{
    std::lock_guard<std::mutex> lock(FlightLogMutex);
    FILE* file = std::fopen("playerbot-flight-recorder.log", "a");
    if (!file)
        return;
    std::fwrite(record.data(), 1, record.size(), file);
    std::fflush(file);
    std::fclose(file);
}
#endif
}

void PlayerbotFlightRecorder::Record(Player* bot, char const* phase,
    std::string const& detail)
{
    // This recorder is intentionally limited to instance bots.  Outdoor
    // random-bot activity would obscure the few seconds surrounding a wipe.
    if (!bot || !bot->IsInWorld() || !bot->GetMap() ||
        !bot->GetMap()->IsDungeon())
        return;

    auto const now = std::chrono::system_clock::now().time_since_epoch();
    auto const milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    uint64 const sequence = FlightSequence.fetch_add(1) + 1;
    size_t const threadId = std::hash<std::thread::id>{}(
        std::this_thread::get_id());

    // Do not inspect GetVictim() here. During encounter teardown that pointer
    // can outlive the despawned creature; stored GUID fields remain safe.
    std::ostringstream line;
    line.setf(std::ios::fixed);
    line.precision(2);
    line << "ts=" << milliseconds
         << " seq=" << sequence
         << " thread=" << threadId
         << " phase=" << CleanField(phase ? phase : "unknown")
         << " bot=" << CleanField(bot->GetName())
         << " guid=" << bot->GetGUID().GetCounter()
         << " map=" << bot->GetMapId()
         << " instance=" << bot->GetInstanceId()
         << " alive=" << (bot->IsAlive() ? 1 : 0)
         << " combat=" << (bot->IsInCombat() ? 1 : 0)
         << " hp=" << bot->GetHealth() << '/' << bot->GetMaxHealth()
         << " target=" << bot->GetTarget().GetCounter()
         << " pos=" << bot->GetPositionX() << ',' << bot->GetPositionY()
         << ',' << bot->GetPositionZ();
    if (!detail.empty())
        line << " detail=" << CleanField(detail);
    line << '\n';

    AppendRecord(line.str());
}
