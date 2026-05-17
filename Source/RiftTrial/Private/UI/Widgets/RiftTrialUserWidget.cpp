// Copyright Yerik Guo


#include "UI/Widgets/RiftTrialUserWidget.h"

void URiftTrialUserWidget::SetWidgetController(UObject* InWidgetController)
{
    WidgetController = InWidgetController;
    WidgetControllerSet();
}
