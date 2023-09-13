import { useCallback, useMemo } from "react";
import { Platform, Vibration } from "react-native";

type FeedbackType = "light" | "medium" | "heavy" | "selection" | "success" | "warning" | "error";

export const useHaptic = (feedbackType: FeedbackType = "selection") => {
    const vibrationDurationMap = {
        light: 10,
        medium: 20,
        heavy: 30,
        selection: 15,
        success: 25,
        warning: 35,
        error: 40
    };

    const hapticHandler = useCallback(
        (type: FeedbackType) => {
            if (Platform.OS !== "web") {
                Vibration.vibrate(vibrationDurationMap[type]);
            }
        },
        [],
    );

    const hapticHandlers = useMemo(
        () => ({
            light: () => hapticHandler("light"),
            medium: () => hapticHandler("medium"),
            heavy: () => hapticHandler("heavy"),
            selection: () => hapticHandler("selection"),
            success: () => hapticHandler("success"),
            warning: () => hapticHandler("warning"),
            error: () => hapticHandler("error")
        }),
        [hapticHandler],
    );

    return hapticHandlers[feedbackType];
};
