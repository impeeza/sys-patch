#define TESLA_INIT_IMPL // If you have more than one file using the tesla header, only define this in the main one
#define STBTT_STATIC
#include <tesla.hpp>    // The Tesla Header
#include <string_view>
#include "minIni/minIni.h"

namespace {

constexpr auto CONFIG_PATH = "/config/sys-patch/config.ini";
constexpr auto LOG_PATH = "/config/sys-patch/log.ini";

auto does_file_exist(const char* path) -> bool {
    Result rc{};
    FsFileSystem fs{};
    FsFile file{};
    char path_buf[FS_MAX_PATH]{};

    if (R_FAILED(fsOpenSdCardFileSystem(&fs))) {
        return false;
    }

    strcpy(path_buf, path);
    rc = fsFsOpenFile(&fs, path_buf, FsOpenMode_Read, &file);
    fsFileClose(&file);
    fsFsClose(&fs);
    return R_SUCCEEDED(rc);
}

// creates a directory, non-recursive!
auto create_dir(const char* path) -> bool {
    Result rc{};
    FsFileSystem fs{};
    char path_buf[FS_MAX_PATH]{};

    if (R_FAILED(fsOpenSdCardFileSystem(&fs))) {
        return false;
    }

    strcpy(path_buf, path);
    rc = fsFsCreateDirectory(&fs, path_buf);
    fsFsClose(&fs);
    return R_SUCCEEDED(rc);
}

struct ConfigEntry {
    ConfigEntry(const char* _section, const char* _key, bool default_value) :
        section{_section}, key{_key}, value{default_value} {
            this->load_value_from_ini();
        }

    void load_value_from_ini() {
        this->value = ini_getbool(this->section, this->key, this->value, CONFIG_PATH);
    }

    auto create_list_item(const char* text) {
        auto item = new tsl::elm::ToggleListItem(text, value);
        item->setStateChangedListener([this](bool new_value){
            this->value = new_value;
            ini_putl(this->section, this->key, this->value, CONFIG_PATH);
        });
        return item;
    }

    const char* const section;
    const char* const key;
    bool value;
};

class GuiOptions final : public tsl::Gui {
public:
    GuiOptions() { }

    tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("sys-patch", VERSION_WITH_HASH);
        auto list = new tsl::elm::List();

        list->addItem(new tsl::elm::CategoryHeader("Options"));
        list->addItem(config_patch_sysmmc.create_list_item("Patch sysMMC"));
        list->addItem(config_patch_emummc.create_list_item("Patch emuMMC"));
        list->addItem(config_logging.create_list_item("Logging"));
        list->addItem(config_version_skip.create_list_item("Version skip"));

        frame->setContent(list);
        return frame;
    }

    ConfigEntry config_patch_sysmmc{"options", "patch_sysmmc", true};
    ConfigEntry config_patch_emummc{"options", "patch_emummc", true};
    ConfigEntry config_logging{"options", "enable_logging", true};
    ConfigEntry config_version_skip{"options", "version_skip", true};
};

class GuiToggle final : public tsl::Gui {
public:
    GuiToggle() { }

    tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("sys-patch", VERSION_WITH_HASH);
        auto list = new tsl::elm::List();

        list->addItem(new tsl::elm::CategoryHeader("FS - 0100000000000000"));
        list->addItem(config_noacidsigchk1.create_list_item("noacidsigchk_1.0.0-9.2.0"));
        list->addItem(config_noacidsigchk2.create_list_item("noacidsigchk_1.0.0-9.2.0"));
        list->addItem(config_noncasigchk1.create_list_item("noncasigchk_1.0.0-3.0.2"));
        list->addItem(config_noncasigchk2.create_list_item("noncasigchk_4.0.0-16.1.0"));
        list->addItem(config_noncasigchk3.create_list_item("noncasigchk_17.0.0+"));
        list->addItem(config_nocntchk1.create_list_item("nocntchk_1.0.0-18.1.0"));
        list->addItem(config_nocntchk2.create_list_item("nocntchk_19.0.0+"));

        list->addItem(new tsl::elm::CategoryHeader("LDR - 0100000000000001"));
        list->addItem(config_noacidsigchk4.create_list_item("noacidsigchk_10.0.0+"));

        list->addItem(new tsl::elm::CategoryHeader("ERPT - 010000000000002B"));
        list->addItem(config_no_erpt.create_list_item("no_erpt"));

        list->addItem(new tsl::elm::CategoryHeader("ES - 0100000000000033"));
        list->addItem(config_es1.create_list_item("es_1.0.0-8.1.1"));
        list->addItem(config_es2.create_list_item("es_9.0.0-11.0.1"));
        list->addItem(config_es3.create_list_item("es_12.0.0-18.1.0"));
        list->addItem(config_es4.create_list_item("es_19.0.0-21.2.0"));
        list->addItem(config_es5.create_list_item("es_22.0.0+"));

        list->addItem(new tsl::elm::CategoryHeader("AM - 0100000000000023"));
        list->addItem(config_am1.create_list_item("am_homebrew_fix_22.0.0+"));

        list->addItem(new tsl::elm::CategoryHeader("OLSC - 010000000000003E"));
        list->addItem(config_olsc1.create_list_item("olsc_6.0.0-14.1.2"));
        list->addItem(config_olsc2.create_list_item("olsc_15.0.0-18.1.0"));
        list->addItem(config_olsc3.create_list_item("olsc_19.0.0+"));

        list->addItem(new tsl::elm::CategoryHeader("NIFM - 010000000000000F"));
        list->addItem(config_ctest1.create_list_item("ctest_1.0.0-19.0.1"));
        list->addItem(config_ctest2.create_list_item("ctest_20.0.0+"));

        list->addItem(new tsl::elm::CategoryHeader("NIM - 0100000000000025"));
        list->addItem(config_nim1.create_list_item("blankcal0crashfix_17.0.0+"));
        list->addItem(config_nim_fw1.create_list_item("blockfirmwareupdates_1.0.0-5.1.0"));
        list->addItem(config_nim_fw2.create_list_item("blockfirmwareupdates_6.0.0-6.2.0"));
		list->addItem(config_nim_fw3.create_list_item("blockfirmwareupdates_7.0.0-10.2.0"));
        list->addItem(config_nim_fw4.create_list_item("blockfirmwareupdates_11.0.0-11.0.1"));
        list->addItem(config_nim_fw5.create_list_item("blockfirmwareupdates_12.0.0+"));

        frame->setContent(list);
        return frame;
    }

    ConfigEntry config_noacidsigchk1{"fs", "noacidsigchk_1.0.0-9.2.0", true};
    ConfigEntry config_noacidsigchk2{"fs", "noacidsigchk_1.0.0-9.2.0", true};
    ConfigEntry config_noncasigchk1{"fs", "noncasigchk_1.0.0-3.0.2", true};
    ConfigEntry config_noncasigchk2{"fs", "noncasigchk_4.0.0-16.1.0", true};
    ConfigEntry config_noncasigchk3{"fs", "noncasigchk_17.0.0+", true};
    ConfigEntry config_nocntchk1{"fs", "nocntchk_1.0.0-18.1.0", true};
    ConfigEntry config_nocntchk2{"fs", "nocntchk_19.0.0+", true};
    ConfigEntry config_noacidsigchk4{"ldr", "noacidsigchk_10.0.0+", true};
    ConfigEntry config_no_erpt{"erpt", "no_erpt", true};
    ConfigEntry config_es1{"es", "es_1.0.0-8.1.1", true};
    ConfigEntry config_es2{"es", "es_9.0.0-11.0.1", true};
    ConfigEntry config_es3{"es", "es_12.0.0-18.1.0", true};
    ConfigEntry config_es4{"es", "es_19.0.0-21.2.0", true};
    ConfigEntry config_es5{"es", "es_22.0.0+", true};
    ConfigEntry config_am1{"am", "am_homebrew_fix_22.0.0+ ", true};
    ConfigEntry config_olsc1{"olsc", "olsc_6.0.0-14.1.2", true};
    ConfigEntry config_olsc2{"olsc", "olsc_15.0.0-18.1.0", true};
    ConfigEntry config_olsc3{"olsc", "olsc_19.0.0+", true};
    ConfigEntry config_ctest1{"nifm", "ctest_1.0.0-19.0.1", true};
    ConfigEntry config_ctest2{"nifm", "ctest_20.0.0+", true};
    ConfigEntry config_nim1{"nim", "blankcal0crashfix_17.0.0+", true};
    ConfigEntry config_nim_fw1{"nim", "blockfirmwareupdates_1.0.0-5.1.0", true};
    ConfigEntry config_nim_fw2{"nim", "blockfirmwareupdates_6.0.0-6.2.0", true};
    ConfigEntry config_nim_fw3{"nim", "blockfirmwareupdates_7.0.0-10.2.0", true};
    ConfigEntry config_nim_fw4{"nim", "blockfirmwareupdates_11.0.0-11.0.1", true};
    ConfigEntry config_nim_fw5{"nim", "blockfirmwareupdates_12.0.0+", true};
};

class GuiLog final : public tsl::Gui {
public:
    GuiLog() { }

    tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("sys-patch", VERSION_WITH_HASH);
        auto list = new tsl::elm::List();

        if (does_file_exist(LOG_PATH)) {
            struct CallbackUser {
                tsl::elm::List* list;
                std::string last_section;
            } callback_userdata{list};

            ini_browse([](const mTCHAR *Section, const mTCHAR *Key, const mTCHAR *Value, void *UserData){
                auto user = (CallbackUser*)UserData;
                std::string_view value{Value};

                if (value == "Skipped") {
                    return 1;
                }

                if (user->last_section != Section) {
                    user->last_section = Section;
                    user->list->addItem(new tsl::elm::CategoryHeader("Log: " + user->last_section));
                }

                #define F(x) ((x) >> 4) // 8bit -> 4bit
                constexpr tsl::Color colour_syspatch{F(0), F(255), F(200), F(255)};
                constexpr tsl::Color colour_file{F(255), F(177), F(66), F(255)};
                constexpr tsl::Color colour_unpatched{F(250), F(90), F(58), F(255)};
                #undef F

                if (value.starts_with("Patched")) {
                    auto *item = new tsl::elm::ListItem(Key);
                    item->setValue("Patched", true);
                    user->list->addItem(item);
                } else if (value.starts_with("Unpatched") || value.starts_with("Disabled")) {
                    auto *item = new tsl::elm::ListItem(Key);
                    item->setValue(Value, true);
                    user->list->addItem(item);
                } else if (user->last_section == "stats") {
                    auto *item = new tsl::elm::ListItem(Key);
                    item->setValue(Value, true);
                    user->list->addItem(item);
                } else {
                    auto *item = new tsl::elm::ListItem(Key);
                    item->setValue(Value, true);
                    user->list->addItem(item);
                }

                return 1;
            }, &callback_userdata, LOG_PATH);
        } else {
            list->addItem(new tsl::elm::ListItem("No log found!"));
        }

        frame->setContent(list);
        return frame;
    }
};

class GuiMain final : public tsl::Gui {
public:
    GuiMain() { }

    tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("sys-patch", VERSION_WITH_HASH);
        auto list = new tsl::elm::List();

        auto options = new tsl::elm::ListItem("Options");
        auto toggle = new tsl::elm::ListItem("Toggle patches");
        auto log = new tsl::elm::ListItem("Log");

        options->setClickListener([](u64 keys) -> bool {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<GuiOptions>();
                return true;
            }
            return false;
        });

        toggle->setClickListener([](u64 keys) -> bool {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<GuiToggle>();
                return true;
            }
            return false;
        });

        log->setClickListener([](u64 keys) -> bool {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<GuiLog>();
                return true;
            }
            return false;
        });

        list->addItem(new tsl::elm::CategoryHeader("Menu"));
        list->addItem(options);
        list->addItem(toggle);
        list->addItem(log);

        frame->setContent(list);
        return frame;
    }
};

// libtesla already initialized fs, hid, pl, pmdmnt, hid:sys and set:sys
class SysPatchOverlay final : public tsl::Overlay {
public:
    std::unique_ptr<tsl::Gui> loadInitialGui() override {
        return initially<GuiMain>();
    }
};

} // namespace

int main(int argc, char **argv) {
    create_dir("/config/");
    create_dir("/config/sys-patch/");
    return tsl::loop<SysPatchOverlay>(argc, argv);
}
