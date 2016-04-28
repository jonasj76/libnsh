#include <stdlib.h>
#include <check.h>
#include <nsh.h>

#define oid_netSnmpPlaypen 1, 3, 6, 1, 4, 1, 8072, 9999, 9999
#define oid_exampleOid     oid_netSnmpPlaypen, 1

nsh_scalar_handler_const(exampleOid, ASN_INTEGER, 10);

START_TEST (test_register_scalar)
{
	int ret;

	ret = nsh_register_scalar_ro(exampleOid);
	ck_assert_int_eq(ret, MIB_REGISTERED_OK);

	ret = nsh_register_scalar_ro(exampleOid);
	ck_assert_int_eq(ret, MIB_DUPLICATE_REGISTRATION);
}
END_TEST

START_TEST (test_register_table_ok)
{
	oid table_oid[] = { oid_exampleOid };
	int index[]     = { ASN_INTEGER };
	int ret;

	ret = nsh_register_table("exampleOid",
				 table_oid, OID_LENGTH (table_oid),
				 0, 0, NULL, 0,
				 NULL, NULL, NULL, NULL, NULL,
				 HANDLER_CAN_RONLY);

	ck_assert_int_eq(ret, MIB_REGISTERED_OK);
}
END_TEST

START_TEST (test_register_table_fail)
{
	oid table_oid[] = { oid_exampleOid };
	int index[]     = { ASN_INTEGER };
	int ret;

	ret = nsh_register_table("exampleOid",
				 NULL, 0,
				 0, 0, NULL, 0,
				 NULL, NULL, NULL, NULL, NULL,
				 HANDLER_CAN_RONLY);

	ck_assert_int_eq(ret, MIB_REGISTRATION_FAILED);
}
END_TEST

Suite* register_suite(void)
{
	Suite *s = suite_create("register");
	TCase *tc_register_scalar = tcase_create("register scalar");
	TCase *tc_register_table  = tcase_create("register table");

	tcase_add_test(tc_register_scalar, test_register_scalar);
	suite_add_tcase(s, tc_register_scalar);

	tcase_add_test(tc_register_table,  test_register_table_ok);
	tcase_add_test(tc_register_table,  test_register_table_fail);
	suite_add_tcase(s, tc_register_table);

	return s;
}

int main(void)
{
	int number_failed;
	Suite *s = register_suite();
	SRunner *sr = srunner_create(s);

	srunner_run_all(sr, CK_VERBOSE);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
