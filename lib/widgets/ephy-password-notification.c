/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 *  Copyright © 2016 Gabriel Ivascu <ivascu.gabriel59@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include "ephy-password-notification.h"

#include "ephy-notification-manager.h"

#include <glib/gi18n.h>

struct _EphyPasswordNotification {
  GtkGrid    parent_instance;

  GtkWidget *note;
  GtkWidget *suggestion;

  char      *user;
};

struct _EphyPasswordNotificationClass {
  GtkGridClass parent_class;
};

enum {
  PROP_0,
  PROP_USER
};

G_DEFINE_TYPE (EphyPasswordNotification, ephy_password_notification, GTK_TYPE_GRID);

static void
ephy_password_notification_constructed (GObject *object)
{
  EphyPasswordNotification *self = EPHY_PASSWORD_NOTIFICATION (object);
  char *account;
  char *text;

  account = g_strdup_printf ("<b>%s</b>", self->user);
  text = g_strdup_printf (_("The password of your Firefox account %s "
                            "seems to have been changed."),
                          account);

  gtk_label_set_markup (GTK_LABEL (self->note), text);
  gtk_label_set_text (GTK_LABEL (self->suggestion),
                      _("Please visit Preferences and sign in with the new "
                        "password to continue the sync process."));

  g_free (account);
  g_free (text);

  G_OBJECT_CLASS (ephy_password_notification_parent_class)->constructed (object);
}

static void
ephy_password_notification_finalize (GObject *object)
{
  EphyPasswordNotification *self = EPHY_PASSWORD_NOTIFICATION (object);

  g_free (self->user);

  G_OBJECT_CLASS (ephy_password_notification_parent_class)->finalize (object);
}

static void
ephy_password_notification_set_property (GObject      *object,
                                         guint         prop_id,
                                         const GValue *value,
                                         GParamSpec   *pspec)
{
  EphyPasswordNotification *self = EPHY_PASSWORD_NOTIFICATION (object);

  switch (prop_id) {
    case PROP_USER:
      self->user = g_value_dup_string (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
ephy_password_notification_get_property (GObject      *object,
                                         guint         prop_id,
                                         GValue       *value,
                                         GParamSpec   *pspec)
{
  EphyPasswordNotification *self = EPHY_PASSWORD_NOTIFICATION (object);

  switch (prop_id) {
    case PROP_USER:
      g_value_set_string (value, self->user);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
ephy_password_notification_init (EphyPasswordNotification *self)
{
  self->note = gtk_label_new (NULL);
  gtk_widget_set_halign (self->note, GTK_ALIGN_CENTER);
  gtk_widget_set_hexpand (self->note, TRUE);
  gtk_grid_attach (GTK_GRID (self), self->note, 0, 0, 1, 1);

  self->suggestion = gtk_label_new (NULL);
  gtk_widget_set_halign (self->suggestion, GTK_ALIGN_CENTER);
  gtk_widget_set_hexpand (self->suggestion, TRUE);
  gtk_grid_attach (GTK_GRID (self), self->suggestion, 0, 1, 1, 1);
}

static void
ephy_password_notification_class_init (EphyPasswordNotificationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = ephy_password_notification_constructed;
  object_class->finalize = ephy_password_notification_finalize;
  object_class->set_property = ephy_password_notification_set_property;
  object_class->get_property = ephy_password_notification_get_property;

  g_object_class_install_property (object_class,
                                   PROP_USER,
                                   g_param_spec_string ("user",
                                                        "User",
                                                        "The email of the signed in user",
                                                        "",
                                                        G_PARAM_STATIC_STRINGS | G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE));
}

EphyPasswordNotification *
ephy_password_notification_new (const char *user)
{
  return g_object_new (EPHY_TYPE_PASSWORD_NOTIFICATION,
                       "column-spacing", 12,
                       "orientation", GTK_ORIENTATION_HORIZONTAL,
                       "user", user,
                       NULL);
}

void
ephy_password_notification_show (EphyPasswordNotification *self)
{
  g_return_if_fail (EPHY_IS_PASSWORD_NOTIFICATION (self));

  ephy_notification_manager_add_notification (ephy_notification_manager_dup_singleton (),
                                              GTK_WIDGET (self));
}