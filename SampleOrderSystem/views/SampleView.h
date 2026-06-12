#pragma once
#include "controllers/SampleController.h"

class SampleView {
public:
    explicit SampleView(SampleController* ctrl);
    void run();
private:
    void showList() const;
    void showSearch() const;
    void showAdd();
    SampleController* m_ctrl;
};
