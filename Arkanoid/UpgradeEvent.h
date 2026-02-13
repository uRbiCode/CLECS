#pragma once

/* Marker Event sent whenever player selects an upgrade.
 * RunControllerSystem listens to this event to end the upgrade selection and continue to the next stage.
 */
struct UpgradeSelectedEvent
{
};